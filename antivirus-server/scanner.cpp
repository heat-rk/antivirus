#include "Scanner.h"
#include "OutgoingMessageBodyError.h"
#include "Message.h"
#include "Utils.h"
#include "MessageMethod.h"
#include "MessageStatus.h"
#include "LogWriter.h"
#include "ScannerConstants.h"

#include <stdio.h>
#include <filesystem>

using namespace Antivirus;

DWORD WINAPI scannerChannelThreadHandler(LPVOID lpvParam) {
    Scanner::ChannelThreadParams* params = 
        reinterpret_cast<Scanner::ChannelThreadParams*>(lpvParam);

    for (;;) {
        params->channel->listen([params](Message message) {
            if (cmpstrs(MessageMethod::E_SCAN_PAUSE, message.method, sizeof(message.method))) {
                params->scanner->pause();
            }
            else if (cmpstrs(MessageMethod::E_SCAN_RESUME, message.method, sizeof(message.method))) {
                params->scanner->resume();
            }
            else if (cmpstrs(MessageMethod::E_SCAN_STOP, message.method, sizeof(message.method))) {
                params->scanner->stop();
            }
        });
    }

    return ERROR_SUCCESS;
}

Scanner::Scanner() {
    m_firstBytesBuffer = ByteBuffer(sizeof(int64_t));

    m_stopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_resumeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    m_channel.init(CHANNEL_TYPE_INTERNAL);

    m_channelThreadParams = new ChannelThreadParams;
    m_channelThreadParams->channel = &m_channel;
    m_channelThreadParams->scanner = this;

    m_scannerChannelThread = CreateThread(
        NULL,
        0,
        scannerChannelThreadHandler,
        m_channelThreadParams,
        0,
        NULL
    );

    if (m_scannerChannelThread == NULL) {
        LogWriter::log("Scanner: Channel thread creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

Scanner::~Scanner() {
    TerminateThread(m_scannerChannelThread, 0);
    delete m_channelThreadParams;
    CloseHandle(m_scannerChannelThread);
}

void Scanner::addRecord(VirusRecord record) {
    if (m_records.find(record.signature.first) == m_records.end()) {
        std::vector<VirusRecord> vec;
        m_records[record.signature.first] = vec;
    }

    m_records[record.signature.first].push_back(record);
}

void Scanner::start(wchar_t* path) {
    LogWriter::log("Scanner: Finding entries...\n");

    findEntries(path);

    LogWriter::log("Scanner: Entries count = %d\n", m_entries.size());

    updateStatus(SCANNING, true);
    
    std::ifstream* file;
    const wchar_t* entry;
    uint64_t length;
    char* bytes;

    for (int i = 0; i < m_entries.size(); i++) {
        entry = m_entries[i].c_str();

        file = new std::ifstream;
        file->open(entry, std::ios::binary);

        if (!file->is_open()) {
            LogWriter::log(L"Scanner:Ifstream:Scan: file is not open\n");
            return;
        }

        file->seekg(0, file->end);
        length = file->tellg();
        file->seekg(0, file->beg);

        bytes = new char[length];
        file->read(bytes, length);

        file->close();
        delete file;

        for (
            uint64_t offset = 0;
            offset + sizeof(int64_t) - 1 < length && m_statuses[i] == NOT_SCANNED;
            offset++
        ) {
            if (WaitForSingleObject(m_resumeEvent, INFINITE) == WAIT_OBJECT_0) {
                int8_t status = NOT_SCANNED;

                if (length < sizeof(int64_t)) {
                    status = SCANNED_NOT_INFECTED;
                }
                else if (scan((int8_t*)bytes, length, offset)) {
                    status = SCANNED_INFECTED;
                }
                else if (offset >= length - sizeof(int64_t)) {
                    status = SCANNED_NOT_INFECTED;
                }

                if (status != NOT_SCANNED) {
                    m_statuses[i] = status;

                    LogWriter::log(
                        L"Scanner: %ls - %ls!\n",
                        m_entries[i].c_str(),
                        status == SCANNED_INFECTED ? L"Infected" : L"Not infected"
                    );

                    delete[] bytes;

                    updateStatus(SCANNING);
                }
            }
        }
    }

    updateStatus(SCANNED, true);
    SetEvent(m_stopEvent);

    WaitForSingleObject(m_stopEvent, INFINITE);
}

void Scanner::pause() {
    ResetEvent(m_resumeEvent);
    updateStatus(PAUSED, true);
    LogWriter::log(L"Scanner: pause\n");
}

void Scanner::resume() {
    SetEvent(m_resumeEvent);
    updateStatus(SCANNING, true);
    LogWriter::log(L"Scanner: resume\n");
}

void Scanner::stop() {
    updateStatus(SCANNED, true);
    LogWriter::log(L"Scanner: stop\n");
    SetEvent(m_stopEvent);
}

bool Scanner::scan(int8_t* bytes, uint64_t length, uint64_t offset) {
    m_firstBytesBuffer.clear();
    m_firstBytesBuffer.put(bytes + offset, sizeof(int64_t));
    int64_t first = m_firstBytesBuffer.getInt64();
    auto pos = m_records.find(first);

    if (pos == m_records.end()) {
        return false;
    }

    VirusRecord record;
    int8_t* hash;

    for (int i = 0; i < pos->second.size(); i++) {
        record = pos->second[i];

        if (record.signature.offset != offset) {
            continue;
        }

        if (offset + record.signature.length >= length) {
            continue;
        }

        m_sha256.update(bytes + offset, record.signature.length);
        hash = (int8_t*)m_sha256.digest();

        for (int j = 0; j < HASH_SIZE; j++) {
            if (record.signature.hash[j] != hash[j]) {
                continue;
            }
        }

        delete[] hash;

        return true;
    }

    return false;
}

void Scanner::findEntries(std::wstring path) {
    std::error_code ec;

    if (std::filesystem::is_directory(path, ec)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            findEntries(entry.path().wstring());
        }
    }

    if (ec) {
        LogWriter::log(L"Scanner:findEntries: Directory checking error(%s)\n");
        return;
    }

    if (std::filesystem::is_regular_file(path, ec)) {
        m_entries.push_back(path);
        m_statuses.push_back(NOT_SCANNED);
    }

    if (ec) {
        LogWriter::log(L"Scanner:findEntries: Regular file checking error(%s)\n");
        return;
    }
}

void Scanner::updateStatus(int8_t status, bool force) {
    if (force) {
        m_scanStatus = status;
    } else if (m_scanStatus != PAUSED) {
        m_scanStatus = status;
    }

    m_cache.save(m_entries, m_statuses, m_scanStatus);
}