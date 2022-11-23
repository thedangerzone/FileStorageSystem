#pragma once
#include "Connection.h"
#include "Requests.h"
#include "Responses.h"

class ClientConnection : public Connection
{
public:
	ClientConnection(const string& serverAddr, const string& serverPort, const GUID& clientId = GUID());
	~ClientConnection();

	GUID registerRequest(std::string& clientName);
	std::string aesKeyRequest(std::string& clientName, std::string& publicKey);
	uint32_t sendFileRequest(GUID& clientId, std::string& fileName, std::string const& encryptyed_content, uint32_t fileCRC);
	void sendCRCValid(GUID& clientId, std::string& fileName);
	void sendCRCInvalid(GUID& clientId, std::string& fileName);
	void sendCRCInvalidDone(GUID& clientId, std::string& fileName);
	
private:
	void requestToSend(RequestPayload& payload);
	void responseToRecieve(ResponsePayload& payload);
	const GUID& clientId;
	static const uint8_t version;
};