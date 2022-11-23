#include "Connection.h"

Connection::Connection() : io_srv(), 
clientSocket(io_srv), resolver(io_srv) {}

Connection::~Connection() {}

void Connection::connectToServer(const string& serverIp, const string& serverPort) {
	boost::asio::connect(clientSocket, resolver.resolve(serverIp, serverPort));
}

void Connection::sendRequest(vector<std::byte> bytes) {
	clientSocket.send(boost::asio::buffer(bytes));
}

vector<std::byte> Connection::recieveRequest(size_t size) {
    auto received_bytes = vector<std::byte>(size);
    size_t read_bytes = read(clientSocket, boost::asio::buffer(received_bytes));

    assert(size == received_bytes.size());
    return received_bytes;
}