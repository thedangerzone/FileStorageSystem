#pragma once
#pragma pack(push, 1)
#include <guiddef.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>

using std::string;
using std::vector;

typedef enum responseCode_t {
    REGSTRATION_SUCCESS = 2100,
    REGSTRATION_FAILURE = 2101,
    RESPONSE_AES_KEY    = 2102,
    RESPONSE_FILE       = 2103,
    SERVER_ERROR        = 2104
} responseCode;

class ResponseHeader {
public:
    uint8_t version;
    uint16_t code;
    uint32_t payloadSize;

    size_t size();
    void parse(vector<std::byte> receivedBytes);
};

class ResponsePayload {
public:
    virtual void parse(vector<std::byte> receivedBytes) = 0;
};

class RegistrationSuccessResponse : public ResponsePayload {
public:
    GUID clientId;
    void parse(vector<std::byte> receivedBytes);
};

struct GetAESResponse : public ResponsePayload {
public:
    GUID clientId;
    string aesKey;
    void parse(vector<std::byte> receivedBytes);
};

struct SentFileResponse : public ResponsePayload {
public:
    GUID clientId;
    string fileName;
    uint32_t cksum;
    uint32_t fileSize;
    void parse(vector<std::byte> receivedBytes);
};

struct CRCValidResponse : public ResponsePayload {
public:
    void parse(vector<std::byte> receivedBytes);
};

struct CRCInvalidResponse : public ResponsePayload {
public:
    void parse(vector<std::byte> receivedBytes);
};

struct CRCInvalidDoneResponse : public ResponsePayload {
public:
    void parse(vector<std::byte> receivedBytes);
};

#pragma pack(pop)