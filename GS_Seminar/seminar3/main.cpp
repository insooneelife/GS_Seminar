#include <iostream>
#include <memory>
#include <vector>
#include <conio.h>
#include "Socket/UDPSocket.h"
#include "Socket/TCPSocket.h"
using namespace std;

// TCP blocking example
namespace tcpblocking
{
	void Server(const string& port)
	{
		cout << "TCP chat server start" << endl;

		// Tcp Socket을 만든다. 
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));
		
		// Port 정보만 이용하여 주소를 만들고 socket에 등록한다.
		// Port만 이용하는 이유는,
		// 자기 자신의 ip를 socket에 등록하는 경우에 ip정보를 따로 받을 필요가 없기 때문이다. 
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		
		// Socket을 수동 대기 모드로 변경시킨다.
		// 최대 수용 인원은 32명
		socket->listen(32);

		// Tcp 통신의 경우 연결을 유지해야하는 
		vector<unique_ptr<TCPSocket>> clients;
		while (1)
		{
			SocketAddress client_address;
			cout << "blocked in accept" << endl;

			clients.emplace_back(socket->accept(client_address));

			for (auto c = begin(clients); c != end(clients); ++c)
			{
				char buffer[10000] = { 0 };
				(*c)->receive(buffer, 10000);
				cout << buffer << endl;
			}
		}
	}

	void Client(const string& address)
	{
		cout << "TCP chat client start" << endl;
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(address));
		
		socket->connect(*server_address);
		while (1)
		{
			string message = "here you go, the message from client!";
			socket->send(message.c_str(), message.size());
		}
	}
}

namespace udpblocking
{
	void Server(const string& port)
	{
		cout << "UDP blocking server start" << endl;
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		//socket->setNoneBlockingMode(true);

		// 통신
		while (1)
		{
			SocketAddress client_address;
			char buffer[1024] = { 0 };
			socket->receiveFrom(buffer, 1024, client_address);

			cout << "[server received] address : "
				<< client_address.toString()
				<< "   data : "
				<< buffer << endl;

			string message = "hello! client im server!";
			socket->sendTo(message.c_str(), message.size(), client_address);

			cout << "process in game logic.." << endl;
		}
	}


	void Client(const string& server_addr)
	{
		cout << "UDP blocking client start" << endl;
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		
		SocketAddress address;
		socket->bind(address);
		//socket->setNoneBlockingMode(true);


		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

		int i = 0;
		while (1)
		{
			if ((i++) % 20 == 0)
			{
				string message = "hello! im client!";
				socket->sendTo(message.c_str(), message.size(), *server_address);

				SocketAddress temp;
				char buffer[1024] = { 0 };
				socket->receiveFrom(buffer, 1024, temp);

				cout << "[client received] address : "
					<< temp.toString()
					<< "   data : "
					<< buffer << endl;
			}
			Sleep(50);
		}
	}
}


namespace chat
{
	// UDP Chat Server
	void Server(const string& port)
	{
		cout << "Chat Server start!" << endl;
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		socket->setNoneBlockingMode(true);

		while (1)
		{
			SocketAddress client_address;
			char buffer[1024] = { 0 };

			int readByteCount = socket->receiveFrom(buffer, 1024, client_address);
			if (readByteCount == 0)
			{
				// nothing to read
			}
			else if (readByteCount == -WSAECONNRESET)
			{
				// port closed on other end, so DC this person immediately
			}
			else if (readByteCount > 0)
			{
				cout << "[server received] address : "
					<< client_address.toString()
					<< "   data : "
					<< buffer << endl;

				socket->sendTo(buffer, readByteCount, client_address);
			}
			else
			{
				// uhoh, error? exit or just keep going?
			}

			// process other logics ..
		}
	}

	// UDP Chat Client
	void Client(const string& server_addr)
	{
		cout << "Chat Client start!" << endl;
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		SocketAddress address;
		socket->bind(address);
		socket->setNoneBlockingMode(true);

		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

		char inputs[128] = { 0 };
		char inputCur = 0;

		while (1)
		{
			if (_kbhit())
			{
				char ch = _getch();
				inputs[inputCur++] = ch;
				cout << ch;

				if (ch == 13)
				{
					inputs[inputCur] = 0;
					socket->sendTo(inputs, inputCur, *server_address);
					inputCur = 0;
				}
			}

			SocketAddress temp;
			char buffer[1024] = { 0 };
			int readByteCount = socket->receiveFrom(buffer, 1024, temp);

			if (readByteCount == 0)
			{
			}
			else if (readByteCount == -WSAECONNRESET)
			{
			}
			else if (readByteCount > 0)
			{
				cout << "[client received] address : "
					<< temp.toString()
					<< "   data : "
					<< buffer << endl;
			}
			else
			{
			}
		}
	}
}




int main(int argc, char * argv[])
{
	// winsock 초기화
	SocketUtil::staticInit();

	//tcpblocking::Server("8000");
	//tcpblocking::Client("127.0.0.1:8000");

	//udpblocking::Server("8000");
	//udpblocking::Client("127.0.0.1:8000");

	//chat::Server("8000");
	chat::Client("127.0.0.1:8000");


	SocketUtil::cleanUp();
	return 0;
}