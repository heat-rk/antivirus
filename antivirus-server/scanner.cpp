#include <stdio.h>
#include <filesystem>

#include "Scanner.h"
#include "OutgoingMessageBodyScan.h"
#include "OutgoingMessageBodyError.h"
#include "Message.h"
#include "Utils.h"
#include "MessageMethod.h"
#include "MessageStatus.h"

using namespace Antivirus;
using namespace std;

DWORD WINAPI scannerThreadHandler(LPVOID lpvParam) {
    Scanner::ThreadParams* params = reinterpret_cast<Scanner::ThreadParams*>(lpvParam);

	for (;;) {
        if (!params->isScanning() || params->queue->empty()) {
            Sleep(1000);
            continue;
        }

        params->queue->back()();
        params->queue->pop_back();
	}
}

Scanner::Scanner() {
	m_scannerThread = INVALID_HANDLE_VALUE;
    m_isScanning = false;
    m_outgoingMessagesHandler = [](Message) {};

    m_threadParams = new ThreadParams;
    m_threadParams->isScanning = [this] { return isScanning(); };
    m_threadParams->queue = &m_scanQueue;

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
        vector<VirusRecord> vec;
        m_records[record.signature.first] = vec;
    }

    m_records[record.signature.first].push_back(record);
}

void Scanner::handleIncomingMessage(Message message) {

}

void Scanner::setOutgoingMessagesHandler(std::function<void(Message)> onMessage) {
    m_outgoingMessagesHandler = onMessage;
}

void Scanner::start(wchar_t* path) {
    if (m_isScanning) {
        char message[] = "Scanner: Scanner is already running\n";

        OutgoingMessageBodyError body;

        copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*) MessageMethod::E_SCAN,
            MessageStatus::E_ERROR,
            &body
        );

        m_outgoingMessagesHandler(response);

        printf(message);

        return;
    }

    m_isScanning = true;

    printf("Finding entries...\n");

    bool infected;
    vector<wstring> entries;
    findEntries(path, &entries);

    printf("Entries count = %d\n", entries.size());

    for (int i = 0; i < entries.size(); i++) {
        const wchar_t* entry = entries[i].c_str();

        addToScanQueue([this, entry, i, entries] {
            wprintf(L"Scanning of %ls ...\n", entry);

            ifstream* file = new ifstream;
            file->open(entry, ios::binary);
            bool infected = scan(file);

            OutgoingMessageBodyScan body;

            body.progress = i;
            body.total = entries.size();
            body.pathLength = wcslen(entry);
            body.path = new wchar_t[body.pathLength];
            copy(entry, entry + body.pathLength, body.path);

            Message response = generateMessage(
                (char*) MessageMethod::E_SCAN,
                MessageStatus::E_OK,
                &body
            );

            if (infected) {
                body.infected = 1;
                wprintf(L"%ls - Infected!\n", entry);
            } else {
                body.infected = 0;
                wprintf(L"%ls - Ok\n", entry);
            }

            m_outgoingMessagesHandler(response);

            delete file;
        });
    }

    waitForScannerThread();
}

void Scanner::pause() {
    m_isScanning = false;
}

void Scanner::resume() {
    m_isScanning = true;
}

void Scanner::stop() {
    m_isScanning = false;
    m_scanQueue.clear();
}

void Scanner::waitForScannerThread() {
    WaitForSingleObject(m_scannerThread, INFINITE);
}

bool Scanner::isScanning() {
    return m_isScanning;
}

bool Scanner::scan(ifstream* file) {
    if (!file->is_open()) {
        char message[] = "Scanner:Ifstream:Scan: file is not open\n";

        OutgoingMessageBodyError body;

        copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*) MessageMethod::E_SCAN,
            MessageStatus::E_ERROR,
            &body
        );

        m_outgoingMessagesHandler(response);

        printf(message);
        
        return false;
    }

    file->seekg(0, file->end);
    uint64_t length = file->tellg();
    file->seekg(0, file->beg);

    char* bytes = new char[length];
    file->read(bytes, length);

    bool result = scan((int8_t*) bytes, length);

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
    vector<VirusRecord> records = m_records[first];
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
        hash = (int8_t*) m_sha256.digest();

        for (int j = 0; j < HASH_SIZE; j++) {
            if (record.signature.hash[j] != hash[j]) {
                continue;
            }
        }

        return true;
    }

    return false;
}

void Scanner::findEntries(wstring path, vector<wstring>* entries) {
    std::error_code ec;

    if (std::filesystem::is_directory(path, ec)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            findEntries(entry.path().wstring(), entries);
        }
    }

    if (ec) {
        char message[] = "Scanner:findEntries: Directory checking error(%s)\n";

        OutgoingMessageBodyError body;

        copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*) MessageMethod::E_SCAN,
            MessageStatus::E_ERROR,
            &body
        );

        m_outgoingMessagesHandler(response);

        printf(message);
        return;
    }

    if (std::filesystem::is_regular_file(path, ec)) {
        entries->push_back(path);
    }

    if (ec) {
        char message[] = "Scanner:findEntries: Regular file checking error(%s)\n";

        OutgoingMessageBodyError body;

        copy(message, message + sizeof(message), body.message);

        Message response = generateMessage(
            (char*) MessageMethod::E_SCAN,
            MessageStatus::E_ERROR,
            &body
        );

        m_outgoingMessagesHandler(response);

        printf(message);
        return;
    }
}

void Scanner::addToScanQueue(function<void()> task) {
    m_scanQueue.insert(m_scanQueue.begin(), task);
}