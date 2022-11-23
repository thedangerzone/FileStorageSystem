#pragma once
#include "ClientConnection.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "Exception.h"
#include "Utils.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Base64Wrapper.h"
#include "crc.h"

typedef enum user_option_e {
	REGISTER			= 1,
	SEND_FILE			= 2,
	CHANGE_FILE			= 3,
	EXIT				= 0
}user_option;


class ClientManager {
public:
	ClientManager();
	~ClientManager();
	void handleRequests();

private:
	void printInstructions();
	void registerRequest();
	void aesKeyRequest();
	void sendFileRequest();
	void changeFile();
	
	void getServerIpPort();
	void setClientInfo();
	void getClientInfo();
	void writeMeInfo();

	std::string getFileName(std::string);

	static const std::string SERVER_INFO_FILE;
	static const std::string CLIENT_INFO_FILE;

	GUID clientId;
	ClientConnection* clientConnection;
	std::string clientName;
	std::string publicKey;
	std::string privateKey;
	std::string aesKey;
	std::string serverIp;
	std::string serverPort;
};