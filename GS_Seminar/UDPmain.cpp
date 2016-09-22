#include <iostream>
#include "seminar2\UDPSocket.h"
using namespace std;

namespace udp
{
	// UDP ����
	void UDPServer()
	{
		// UDP socket ����
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// �ڽ��� �ּ� ����
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// �ڽ��� �ּҸ� socket�� ���
		udp_socket->bind(*sock_address);

		// �ּҸ� ���� �ӽ� ����
		SocketAddress from_address;
		
		// recieve
		char buffer[128] = { 0 };
		udp_socket->receiveFrom(buffer, 128, from_address);
		cout << buffer << endl;;


		delete udp_socket;
		delete sock_address;
	}

	// UDP Ŭ���̾�Ʈ
	void UDPClient()
	{
		// UDP socket ����
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// ���� �ּ�
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
	// winsock �ʱ�ȭ
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