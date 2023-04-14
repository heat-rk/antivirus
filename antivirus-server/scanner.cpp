#include "Scanner.h"
#include "OutgoingMessageBodyScan.h"
#include "OutgoingMessageBodyError.h"
#include "Message.h"
#include "Utils.h"
#include "MessageMethod.h"
#include "MessageStatus.h"
#include "ByteBuffer.h"

#include <stdio.h>
#include <filesystem>

using namespace Antivirus;

DWORD WINAPI scannerThreadHandler(LPVOID lpvParam) {
    Scanner::ThreadParams* params = reinterpret_cast<Scanner::ThreadParams*>(lpvParam);

    params->channel->listen([params](Message message) {
        if (cmpstrs(MessageMethod::E_SCAN_PAUSE, message.method, sizeof(message.method))) {
            params->scanner->pause();
            params->channel->write(
                generateMessage(
                    (char*)MessageMethod::E_SCAN_PAUSE,
                    MessageStatus::E_OK
                )
            );
        }
        else if (cmpstrs(MessageMethod::E_SCAN_STOP, message.method, sizeof(message.method))) {
            params->scanner->stop();
            params->channel->write(
                generateMessage(
                    (char*)MessageMethod::E_SCAN_STOP,
                    MessageStatus::E_OK
                )
            );
        }
    });

    return ERROR_SUCCESS;
}

Scanner::Scanner() {
    m_isActive = true;
    m_isScanning = false;

    m_channel.init();

    m_threadParams = new ThreadParams;
    m_threadParams->channel = &m_channel;
    m_threadParams->scanner = this;

    m_scannerThread = CreateThread(
        NULL,
        0,
        scannerThreadHandler,
        m_threadParams,
        0,
        NULL
    );

    if (m_scannerThread == NULL) {
        printf("Client thread creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

Scanner::~Scanner() {
    delete m_threadParams;
    CloseHandle(m_scannerThread);
}

void Scanner::addRecord(VirusRecord record) {
    if (m_records.find(record.signature.first) == m_records.end()) {
        std::vector<VirusRecord> vec;
        m_records[record.signature.first] = vec;
    }

    m_records[record.signature.first].push_back(record);
}

void Scanner::start(wchar_t* path) {
    if (m_isScanning) {
        char message[] = "Scanner: Scanner is already running\n";

        OutgoingMessageBodyError body;

        std::copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*) MessageMethod::E_SCAN_START,
            MessageStatus::E_ERROR,
            &body
        );

        m_channel.write(response);

        printf(message);

        return;
    }

    m_isScanning = true;
    m_isActive = true;

    printf("Finding entries...\n");

    bool infected;
    std::vector<std::wstring> entries;
    std::vector<std::wstring> viruses;
    findEntries(path, &entries);

    printf("Entries count = %d\n", entries.size());

    for (int i = 0; i < entries.size(); i++) {
        if (!m_isActive) {
            return;
        }

        while (!m_isScanning) {
            Sleep(1000);
        }

        const wchar_t* entry = entries[i].c_str();

        wprintf(L"Scanning of %ls ...\n", entry);

        std::ifstream* file = new std::ifstream;
        file->open(entry, std::ios::binary);
        infected = scan(file);

        OutgoingMessageBodyScan body;

        body.progress = i;
        body.total = entries.size();
        body.pathLength = wcslen(entry);
        body.path = new wchar_t[body.pathLength];
        std::copy(entry, entry + body.pathLength, body.path);

        if (infected) {
            body.infected = 1;
            viruses.push_back(entries[i]);
            wprintf(L"%ls - Infected!\n", entry);
        }
        else {
            body.infected = 0;
            wprintf(L"%ls - Ok\n", entry);
        }

        Message response = generateMessage(
            (char*)MessageMethod::E_SCAN_START,
            MessageStatus::E_OK,
            &body
        );

        m_channel.write(response);

        delete file;
    }

    m_isScanning = false;
    m_isActive = false;

    m_cache.save(viruses);
}

void Scanner::pause() {
    m_isScanning = false;
}

void Scanner::resume() {
    m_isScanning = true;
}

void Scanner::stop() {
    m_isScanning = false;
    m_isActive = false;
}

bool Scanner::isScanning() {
    return m_isScanning;
}

bool Scanner::scan(std::ifstream* file) {
    if (!file->is_open()) {
        char message[] = "Scanner:Ifstream:Scan: file is not open\n";

        OutgoingMessageBodyError body;

        std::copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*)MessageMethod::E_SCAN_START,
            MessageStatus::E_ERROR,
            &body
        );

        m_channel.write(response);

        printf(message);

        return false;
    }

    file->seekg(0, file->end);
    uint64_t length = file->tellg();
    file->seekg(0, file->beg);

    char* bytes = new char[length];
    file->read(bytes, length);

    bool result = scan((int8_t*)bytes, length);

    delete bytes;
    return result;
}

bool Scanner::scan(int8_t* bytes, uint64_t length) {
    for (uint64_t i = 0; i < length; i++) {
        if (scan(bytes, length, i)) {
            return true;
        }
    }

    return false;
}

bool Scanner::scan(int8_t* bytes, uint64_t length, uint64_t offset) {
    ByteBuffer byteBuffer(sizeof(int64_t));
    byteBuffer.put(bytes + offset, sizeof(int64_t));
    int64_t first = byteBuffer.getInt64();
    VirusRecord record;
    std::vector<VirusRecord> records = m_records[first];
    int8_t* hash;

    for (int i = 0; i < records.size(); i++) {
        record = records[i];

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

        return true;
    }

    return false;
}

void Scanner::findEntries(std::wstring path, std::vector<std::wstring>* entries) {
    std::error_code ec;

    if (std::filesystem::is_directory(path, ec)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            findEntries(entry.path().wstring(), entries);
        }
    }

    if (ec) {
        char message[] = "Scanner:findEntries: Directory checking error(%s)\n";

        OutgoingMessageBodyError body;

        std::copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*)MessageMethod::E_SCAN_START,
            MessageStatus::E_ERROR,
            &body
        );

        m_channel.write(response);

        printf(message);
        return;
    }

    if (std::filesystem::is_regular_file(path, ec)) {
        entries->push_back(path);
    }

    if (ec) {
        char message[] = "Scanner:findEntries: Regular file checking error(%s)\n";

        OutgoingMessageBodyError body;

        std::copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*)MessageMethod::E_SCAN_START,
            MessageStatus::E_ERROR,
            &body
        );

        m_channel.write(response);

        printf(message);
        return;
    }
}