#include "Responses.h"
#include "Endiness.h"

constexpr size_t CLIENT_ID_SIZE = 16;
constexpr size_t USERNAME_SIZE = 255;
constexpr size_t FILE_NAME_SIZE = 255;
constexpr size_t AES_KEY_SIZE = 128;

template<typename T>
T pop(vector<std::byte>& response) {
    T var;
    std::byte array[sizeof(T)];
    for (std::byte& b : array) {
        b = response.front();
        response.erase(response.begin());
    }
    memcpy(&var, &array[0], sizeof(T));
    return littleEndian<T>(var);
}

string popStr(vector<std::byte>& response, size_t size) {
    if (size > response.size()) {
        throw std::length_error("String size is bigger than expected");
    }
    string output;
    output.resize(size, 0);
    for (char& c : output) {
        c = static_cast<char>(response.front());
        response.erase(response.begin());
    }
    return output;
}

size_t ResponseHeader::size() {
    return sizeof(version) + sizeof(code) + sizeof(payloadSize);
}

void ResponseHeader::parse(vector<std::byte> receivedBytes) {
    version = pop<uint8_t>(receivedBytes);
    code = pop<uint16_t>(receivedBytes);
    payloadSize = pop<uint32_t>(receivedBytes);
}

void RegistrationSuccessResponse::parse(vector<std::byte> receivedBytes) {
    clientId = pop<GUID>(receivedBytes);
}

void GetAESResponse::parse(vector<std::byte> receivedBytes) {
    clientId = pop<GUID>(receivedBytes);
    aesKey = popStr(receivedBytes, AES_KEY_SIZE);
}

void SentFileResponse::parse(vector<std::byte> receivedBytes) {
    clientId = pop<GUID>(receivedBytes);
    fileSize = pop<uint32_t>(receivedBytes);
    fileName = popStr(receivedBytes, FILE_NAME_SIZE);
    cksum = pop<uint32_t>(receivedBytes); 
}

void CRCValidResponse::parse(vector<std::byte> receivedBytes) { }
void CRCInvalidResponse::parse(vector<std::byte> receivedBytes) { }
void CRCInvalidDoneResponse::parse(vector<std::byte> receivedBytes) { }