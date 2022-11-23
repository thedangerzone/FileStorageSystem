#pragma once
#include <guiddef.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>

using std::string;
using std::vector;

enum class requestCode : uint16_t {
    REQUEST_REGSTRATION      = 1100,
    REQUEST_AES_KEY          = 1101,
    REQUEST_SEND_FILE        = 1103,
    REQUEST_CRC_VALID        = 1104,
    REQUEST_CRC_INVALID      = 1105,
    REQUEST_CRC_INVALID_DONE = 1106
};

#pragma pack(push, 1)
class RequestHeader {
private:
    GUID clientId;
    uint8_t version;
    requestCode code;
    uint32_t payloadSize;

public:
    RequestHeader(GUID clientId, uint8_t version, requestCode code, uint32_t payloadSize);
    vector<std::byte> build();
};

class RequestPayload {
protected:
    requestCode code;

public:
    virtual vector<std::byte> build() const = 0;
    virtual requestCode getCode() const = 0;
};

class RegistrationRequest : public RequestPayload {
private:
    string name;

public:
    RegistrationRequest(string name);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

class GetAesKeyRequest : public RequestPayload {
private:
    string name;
    string publicKey;


public:
    GetAesKeyRequest(string name, string publicKey);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

struct SendFileRequest : public RequestPayload {
private:
    GUID clientId;
    uint32_t contentSize;
    string fileName;
    string fileContent;

public:
    SendFileRequest(GUID clientId, string fileName, string fileContent);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

class SendCRCValid : public RequestPayload {
private:
    GUID clientId;
    string fileName;


public:
    SendCRCValid(GUID clientId, string fileName);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

class SendCRCInvalid : public RequestPayload {
private:
    GUID clientId;
    string fileName;


public:
    SendCRCInvalid(GUID clientId, string fileName);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

class SendCRCInvalidDone : public RequestPayload {
private:
    GUID clientId;
    string fileName;


public:
    SendCRCInvalidDone(GUID clientId, string fileName);
    vector<std::byte> build() const;
    requestCode getCode() const;
};

#pragma pack(pop)