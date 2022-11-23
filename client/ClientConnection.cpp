#include "ClientConnection.h"
#include "Exception.h"
#include "Base64Wrapper.h"
#include <combaseapi.h>


const uint8_t ClientConnection::version = 3;
std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
        output.push_back(' ');
    }
    return output;
}

ClientConnection::ClientConnection(const std::string& serverIp, const std::string& serverPort, 
                                    const GUID& clientId) : clientId(clientId) {
    connectToServer(serverIp, serverPort);
}

ClientConnection::~ClientConnection() {}

void ClientConnection::requestToSend(RequestPayload& payload) {
    vector<std::byte> requestPayload = payload.build();
    vector<std::byte> requestHeader = 
        RequestHeader(clientId, version, payload.getCode(),
            static_cast<uint32_t>(requestPayload.size())).build();

    sendRequest(requestHeader);
    
    if (requestPayload.size() != 0) {
        sendRequest(requestPayload);
    }
}

void ClientConnection::responseToRecieve(ResponsePayload& payload) {
    ResponseHeader response_header;
    response_header.parse(recieveRequest(response_header.size()));

    if (response_header.code == SERVER_ERROR || response_header.code == REGSTRATION_FAILURE) {
        throw ServerError();
    }
    
    if (response_header.payloadSize != 0)
        payload.parse(recieveRequest(response_header.payloadSize));
}

GUID ClientConnection::registerRequest(std::string& clientName) {

    RegistrationRequest request = RegistrationRequest(clientName);
    requestToSend(request);

    RegistrationSuccessResponse response;
    try {
        responseToRecieve(response);
    }
    catch (const ServerError&) {
        throw(ServerError());
    }

    return response.clientId;
}

std::string ClientConnection::aesKeyRequest(std::string& clientName, std::string& publicKey) {
    GetAesKeyRequest request = GetAesKeyRequest(clientName, publicKey);
    requestToSend(request);

    GetAESResponse response;
    try {
        responseToRecieve(response);
    }
    catch (const ServerError&) {
        throw(ServerError());
    }
    return response.aesKey;
}

uint32_t ClientConnection::sendFileRequest(GUID& clientId, std::string& fileName, std::string const& encryptyed_content, uint32_t fileCRC) {
    SendFileRequest request = SendFileRequest(clientId, fileName, encryptyed_content);
    requestToSend(request);

    SentFileResponse response;
    try {
        responseToRecieve(response);
    }
    catch (const ServerError&) {
        throw(ServerError());
    }
    
    if (response.clientId != clientId) 
        throw(ServerError());
    
    return response.cksum;
}

void ClientConnection::sendCRCValid(GUID& clientId, std::string& fileName) {
    SendCRCValid request = SendCRCValid(clientId, fileName);
    requestToSend(request);

    CRCValidResponse response;
}

void ClientConnection::sendCRCInvalid(GUID& clientId, std::string& fileName) {
    SendCRCInvalid request = SendCRCInvalid(clientId, fileName);
    requestToSend(request);

    CRCInvalidResponse response;
}

void ClientConnection::sendCRCInvalidDone(GUID& clientId, std::string& fileName) {
    SendCRCInvalidDone request = SendCRCInvalidDone(clientId, fileName);
    requestToSend(request);

    CRCInvalidDoneResponse response;
}



