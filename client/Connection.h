#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include <boost/asio.hpp>
#include <climits>

using boost::asio::ip::tcp;
using std::string;
using std::vector;


class Connection
{
public:
	Connection();
	~Connection();

protected:
	void connectToServer(const string& serverIp, const string& serverPort);
	void sendRequest(vector<std::byte> bytes);
	vector<std::byte> recieveRequest(size_t size);

	boost::asio::io_service io_srv;
	tcp::socket clientSocket;
	tcp::resolver resolver;
};