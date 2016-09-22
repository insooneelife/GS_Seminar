#include <iostream>
#include "seminar2\UDPSocket.h"
using namespace std;

namespace udp
{
	// UDP 서버
	void UDPServer()
	{
		// UDP socket 생성
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// 자신의 주소 생성
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// 자신의 주소를 socket에 등록
		udp_socket->bind(*sock_address);

		// 주소를 담을 임시 공간
		SocketAddress from_address;
		
		// recieve
		char buffer[128] = { 0 };
		udp_socket->receiveFrom(buffer, 128, from_address);
		cout << buffer << endl;;


		delete udp_socket;
		delete sock_address;
	}

	// UDP 클라이언트
	void UDPClient()
	{
		// UDP socket 생성
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// 보낼 주소
		SocketAddress* to_address = SocketAddress::createFromString("127.0.0.1:8000");
		
		// send
		std::string message = "Hello UDP world!";
		udp_socket->sendTo(message.c_str(), message.size(), *to_address);


		delete udp_socket;
		delete to_address;
	}
}


int main(int argc, char * argv[])
{
	// winsock 초기화
	SocketUtil::staticInit();

	if (argc == 2)
	{
		string str = argv[1];
		if (str == "Client")
		{
			udp::UDPClient();
			return 0;
		}
		else if (str == "Server")
		{
			udp::UDPServer();
			return 0;
		}
	}

	std::cout << "input argument = \"Client\" or \"Server\"";

	SocketUtil::cleanUp();

	return 0;
}