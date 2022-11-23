#include "ClientManager.h"
#include "Endiness.h"


namespace fs = std::filesystem;
using std::ifstream;
using std::ofstream;
using std::cin;
using std::cout;
using std::endl;

const std::string ClientManager::SERVER_INFO_FILE = "transfer.info";
const std::string ClientManager::CLIENT_INFO_FILE = "me.info";


ClientManager::ClientManager() : clientName(std::string()) {
    getServerIpPort();
    setClientInfo();
}

void ClientManager::setClientInfo() {
    getClientInfo();
    if (!clientName.empty())
    {
        cout << "Hello again " << clientName << "!" << endl << endl;
        RSAPrivateWrapper rsapriv(privateKey);
        publicKey = rsapriv.getPublicKey();
    }

    else {
        RSAPrivateWrapper rsapriv;
        publicKey = rsapriv.getPublicKey();
        privateKey = rsapriv.getPrivateKey();
    }
}

void ClientManager::writeMeInfo() {
    ofstream meInfoFile(CLIENT_INFO_FILE);
    meInfoFile << clientName << endl;
    meInfoFile << hexEncode(clientId) << endl;
    meInfoFile << Base64Wrapper::encode(privateKey);
    meInfoFile.close();
}

void ClientManager::getClientInfo() {
    ifstream meInfoFile(CLIENT_INFO_FILE);
    std::string line;
    std::string encodedKey;

    // File does not exists
    if (meInfoFile.fail()) {
        return;
    }

    // Get client name(first line of file)
    if (!getline(meInfoFile, clientName)) {
        return;
    }

    
    getline(meInfoFile, line);
    clientId = hexDecode(line);

    while (getline(meInfoFile, line))
    {
        encodedKey += line;
    }

    privateKey = Base64Wrapper::decode(encodedKey);

    meInfoFile.close();
}

void ClientManager::registerRequest() {

    ifstream meInfoFile(CLIENT_INFO_FILE);
    if (meInfoFile.good()) {
        cout << "ME file already exists - cannot register new client" << endl;
        meInfoFile.close();
        return;
    }

    ifstream transferInfoFile(SERVER_INFO_FILE);
    
    // The client name is in the second row of transfer.info file
    std::getline(transferInfoFile, clientName);
    std::getline(transferInfoFile, clientName); 


    clientConnection = new ClientConnection(serverIp, serverPort);
    clientId = clientConnection->registerRequest(clientName);
    delete(clientConnection);

    writeMeInfo();
}

void ClientManager::aesKeyRequest() {
    clientConnection = new ClientConnection(serverIp, serverPort, clientId);
    aesKey = clientConnection->aesKeyRequest(clientName, publicKey);

    // Decrypt the encrypted synch key from server 
    RSAPrivateWrapper rsapriv(privateKey);
    aesKey = rsapriv.decrypt(aesKey);
    delete(clientConnection);
}

void ClientManager::sendFileRequest() {
    std::string fileContent;

    std::string filePath;
    ifstream file(SERVER_INFO_FILE);
    // Get the file's third line - the file path to send
    //try {
        std::getline(file, filePath);
        std::getline(file, filePath);
        std::getline(file, filePath);
   // }
   // catch 

    file.close();


    ifstream fileToSend(filePath, std::ios::binary);
    // Check if file exists
    if (fileToSend.fail()) {
        cout << "File path in transfer.info not found" << endl;
        return;
    }
    cout << "File path: " << filePath << endl;
    // Get the symethric key if we didn't request it yet
    if (aesKey.empty()) {
        this->aesKeyRequest();
    }
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(fileToSend), {});
    fileToSend.close();

    if (buffer.size() > INT_MAX) {
        cout << "File size too big" << endl;
        return;
    }

    uint32_t crc = calcCRC(buffer);

    AESWrapper aes = AESWrapper((const unsigned char*)aesKey.c_str(), AESWrapper::DEFAULT_KEYLENGTH);
    std::string ciphertext = aes.encrypt(reinterpret_cast<char*>(buffer.data()), buffer.size());
    std::string encoded_cipher = Base64Wrapper::encode(ciphertext);
    std::string fileName = this->getFileName(filePath);

    clientConnection = new ClientConnection(serverIp, serverPort, clientId);
    cout << "send file\n" << endl;
    uint32_t server_crc = clientConnection->sendFileRequest(clientId, fileName, encoded_cipher, crc);
    //delete(clientConnection);
    int sendFileTries = 3;
    server_crc = 2;
    while (sendFileTries-- != 0) {
        if (server_crc != crc) {
            cout << "send crc invald\n";
            clientConnection = new ClientConnection(serverIp, serverPort, clientId);
            clientConnection->sendCRCInvalid(clientId, fileName);
            //delete(clientConnection);
            cout << "send file\n";
            clientConnection = new ClientConnection(serverIp, serverPort, clientId);
            server_crc = clientConnection->sendFileRequest(clientId, fileName, encoded_cipher, crc);
            //delete(clientConnection);
            cout << "my crc: " << crc << endl;
            cout << "server crc: " << server_crc << endl;
            server_crc = 2;
        }
        else {
            cout << "send crc valid\n";
            clientConnection->sendCRCValid(clientId, fileName);
            break;
        }
    }

    if (server_crc != crc) {
        cout << "send crc done\n";
        clientConnection->sendCRCInvalidDone(clientId, fileName);
        cout << "File was not verified in server" << endl;
    }
    else
        cout << "File was verified in server" << endl;

    delete(clientConnection);
}

void ClientManager::changeFile() {
    std::string serverIP, filePath;
    ifstream readFile(SERVER_INFO_FILE);
  
    std::getline(readFile, serverIP);  // Get the server IP
    readFile.close();

    cout << "Enter file path to update the next file to send" << endl;
    cin >> filePath;

    ofstream writeFile(SERVER_INFO_FILE);
    writeFile << serverIP << endl;
    writeFile << clientName << endl;
    writeFile << filePath << endl;
}


void ClientManager::getServerIpPort() {

    ifstream inFile(SERVER_INFO_FILE);
    if (inFile.fail()) {
        throw(ClientError("transfer.info file not exists."));
    }
    std::string line, ip, port;
    getline(inFile, line);

    std::string delimiter = ":";
    serverIp = line.substr(0, line.find(delimiter));
    serverPort = line.substr(line.find(delimiter) + 1);
}

void ClientManager::printInstructions() {
    cout << endl << "Server file storing system is at your service! " << endl;
    cout << "Please enter the number to instruct the system! " << endl << endl;
    cout << "1) Register" << endl;
    cout << "2) Send file written in transfer.info" << endl;
    cout << "3) Change file in transfer.info" << endl;
    cout << "0) Exit client" << endl;
    cout << "? ";
}

void ClientManager::handleRequests() {
    unsigned int code = EXIT;

    do {
        printInstructions();
        cin >> code;
        try {
            switch (code) {
            case REGISTER:
                registerRequest();
                break;

            case SEND_FILE:
                sendFileRequest();
                break;

            case CHANGE_FILE:
                changeFile();
                break;

            case EXIT:
                break;

            default:
                break;
            }
        }
        catch (const ServerError& e) {
            std::cout << e.what() << std::endl;
        }
    } while (code != EXIT);
}

std::string ClientManager::getFileName(std::string filePath) {
    std::string fileName = "";
    for (char ch : filePath) {
        fileName += ch;
        // If the path contains slash after it then it is a library
        if (ch == '\\' || ch == '/')
            fileName = "";
    }
    return fileName;
}


ClientManager::~ClientManager() {}
