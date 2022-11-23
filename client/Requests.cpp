#include "Requests.h"
#include "Endiness.h"

constexpr size_t CLIENT_ID_SIZE = 16;
constexpr size_t USERNAME_SIZE = 255;
constexpr size_t FILE_NAME_SIZE = 255;
constexpr size_t PUBLIC_KEY_SIZE = 160;

template<typename T>
void push(vector<std::byte>& request, T var) {
    std::byte array[sizeof(T)];
    T le = littleEndian<T>(var);
    memcpy(&array[0], &le, sizeof(T));
    for (const std::byte b : array) {
        request.push_back(b);
    }
}

void pushStr(vector<std::byte>& request, string str) {
    for (const char c : str) {
        request.push_back(static_cast<std::byte>(c));
    }
}

void pushStr(vector<std::byte>& request, string str, size_t size) {
    str.resize(size, 0);
    pushStr(request, str);
}

RequestHeader::RequestHeader(GUID clientId, uint8_t version,
    requestCode code, uint32_t payloadSize) :
    clientId(clientId), version(version),
    code(code), payloadSize(payloadSize) {}

vector<std::byte> RequestHeader::build() {
    vector<std::byte> header;
    push<GUID>(header, clientId);
    push<uint8_t>(header, version);
    push<requestCode>(header, code);
    push<uint32_t>(header, payloadSize);

    return header;
}

RegistrationRequest::RegistrationRequest(string name) : name(name) {}

vector<std::byte> RegistrationRequest::build() const {
    vector<std::byte> payload;
    pushStr(payload, name, USERNAME_SIZE);

    return payload;
}

requestCode RegistrationRequest::getCode() const {
    return requestCode::REQUEST_REGSTRATION;
}

GetAesKeyRequest::GetAesKeyRequest(string name,
    string publicKey) : name(name), publicKey(publicKey) {}

vector<std::byte> GetAesKeyRequest::build() const {
    vector<std::byte> payload;
    pushStr(payload, name, USERNAME_SIZE);
    pushStr(payload, publicKey, PUBLIC_KEY_SIZE);

    return payload;
}

requestCode GetAesKeyRequest::getCode() const {
    return requestCode::REQUEST_AES_KEY;
}

SendFileRequest::SendFileRequest(GUID clientId,
    string fileName, string fileContent) : clientId(clientId),
    contentSize(fileContent.size()), fileName(fileName), fileContent(fileContent) {}

vector<std::byte> SendFileRequest::build() const {
    vector<std::byte> payload;
    push<GUID>(payload, clientId);
    push<uint32_t>(payload, contentSize);
    pushStr(payload, fileName, FILE_NAME_SIZE);
    pushStr(payload, fileContent);

    return payload;
}

requestCode SendFileRequest::getCode() const {
    return requestCode::REQUEST_SEND_FILE;
}

SendCRCValid::SendCRCValid(GUID clientId,
    string fileName) : clientId(clientId), fileName(fileName) {}

vector<std::byte> SendCRCValid::build() const {
    vector<std::byte> payload;
    push<GUID>(payload, clientId);
    pushStr(payload, fileName, FILE_NAME_SIZE);

    return payload;
}

requestCode SendCRCValid::getCode() const {
    return requestCode::REQUEST_CRC_VALID;
}


SendCRCInvalid::SendCRCInvalid(GUID clientId,
    string fileName) : clientId(clientId), fileName(fileName) {}

vector<std::byte> SendCRCInvalid::build() const {
    vector<std::byte> payload;
    push<GUID>(payload, clientId);
    pushStr(payload, fileName, FILE_NAME_SIZE);

    return payload;
}

requestCode SendCRCInvalid::getCode() const {
    return requestCode::REQUEST_CRC_INVALID;
}

SendCRCInvalidDone::SendCRCInvalidDone(GUID clientId,
    string fileName) : clientId(clientId), fileName(fileName) {}

vector<std::byte> SendCRCInvalidDone::build() const {
    vector<std::byte> payload;
    push<GUID>(payload, clientId);
    pushStr(payload, fileName, FILE_NAME_SIZE);

    return payload;
}

requestCode SendCRCInvalidDone::getCode() const {
    return requestCode::REQUEST_CRC_INVALID_DONE;
}