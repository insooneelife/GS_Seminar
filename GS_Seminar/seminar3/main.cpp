#include <iostream>
#include <memory>
#include "Socket/UDPSocket.h"
using namespace std;


// UDP Chat Server
void Server(const string& server_addr)
{
	cout << "Chat Server start@" << endl;

	unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
	unique_ptr<SocketAddress> address(SocketAddress::createFromString(server_addr));
	socket->bind(*address);
	socket->setNoneBlockingMode(true);

	while (1)
	{
		SocketAddress from_address;
		char buffer[128] = { 0 };
		socket->receiveFrom(buffer, 128, from_address);

		cout << "recv from : " << from_address.toString() << "   data : " << buffer << endl;;

		std::string message = "asshole";
		socket->sendTo(message.c_str(), message.size(), from_address);
	}
}

// UDP Chat Client
void Client(const string& server_addr)
{
	cout << "Chat Client start@" << endl;

	unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

	SocketAddress address;
	socket->bind(address);
	std::cout<<"address : " << address.toString();

	socket->setNoneBlockingMode(true);

	unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

	// Send login packet
	std::string message = "<command>:login";
	socket->sendTo(message.c_str(), message.size(), *server_address);

	int i = 0;
	while (i++ <10)
	{
		//cin >> message;

		//if (message == "quit")
			//break;

		socket->sendTo(message.c_str(), message.size(), *server_address);
	}

	
	//closesocket(socket->_socket);

	std::cout << "@@@";

	

	// Send logout packet
	//message = "<command>:logout";
	//socket->sendTo(message.c_str(), message.size(), *server_address);
	
}



int main(int argc, char * argv[])
{
	// winsock √ ±‚»≠
	SocketUtil::staticInit();

	Client("127.0.0.1:8000");

	//Server("127.0.0.1:8000");

	/*if (argc == 3)
	{
		string str = argv[1];
		string address = argv[2];
		if (str == "Client")
		{
			Client(address);
			return 0;
		}
		else if (str == "Server")
		{
			Server(address);
			return 0;
		}
	}

	std::cout << "input argument = \"Client\" ip:port  or  \"Server\" ip:port";
	*/
	SocketUtil::cleanUp();
	return 0;
}