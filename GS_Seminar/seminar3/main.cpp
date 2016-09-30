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
	// 이전 예제에서 socket을 통해 send/recv 까지 구현해 보았다.
	// 그렇다면 이제 여러 명의 client와 통신하는 server를 구현해 보자.
	// 하지만 막상 아래와 같이 구현을 해 보면 문제가 생기는 것을 알 수 있을 것이다.
	// 기본적으로 accept, send, recv 함수들은 수행하면 현재 main thread를 blocking 상태에 들어가게 하기 때문에,
	// server에서 한 client에게 recv를 하고 있을때 다른 client들의 통신은 무시될 수 밖에 없게 된다. (accept도 마찬가지)
	// 그렇기 때문에 socket을 non-blocking 상태로 바꾸어주어 문제를 보통 해결한다.

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

		// Tcp 통신의 경우 연결을 유지해야하기 때문에,
		// 현재 연결을 성공적으로 수행한 socket들을 갖고 있어야 한다.
		vector<unique_ptr<TCPSocket>> clients;

		while (1)
		{
			SocketAddress client_address;
			cout << "blocked in accept" << endl;

			// accept를 통해 새 연결이 성공적으로 받으면,
			// 연결을 요청한 client와 통신하는 socket을 반환하여 vector에 넣는다.
			clients.emplace_back(socket->accept(client_address));

			// 모든 vector를 순회하면서 data를 recv 한다.
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

		// TCP socket을 만든다.
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));

		// 연결할 server의 주소
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(address));
		
		// 연결을 요청한다.
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
	// Udp socket에 경우, tcp와 달리 하나의 socket을 통해 여러 client들의 data를 읽어올 수 있기 때문에,
	// blocking 상태에 빠지더라도 tcp처럼 다른 client들이 통신이 불가능해지는 상황은 생기지 않는다.
	// 하지만 socket이 하나더라도, recv를 통해 blocking 상태에 빠졌을 때는 server의 작업을 수행할 수 없게 된다.
	// 그러므로 결국 udp socket도 blocking mode로는 게임을 구현하기가 힘들다.

	// Socket은 default로 blocking 모드로 설정되어 있기 때문에 udp의 socket도 blocking socket이다.
	// ioctlsocket 함수를 통해 socket을 non-blocking 모드로 바꿀 수 있는데,
	// 이 과정을 UDPsocket class 내부에 setNoneBlockingMode 함수를 통해 구현해 두었다.

	// Socket이 non-blocking 모드에 들어가게 되면 더 이상 send/recv를 해도 blocking 상태에 들어가지 않는다.
	// Data가 준비되어 있으면 가져오고, 준비되어 있지 않다면 바로 넘어간다.
	// 그렇다면 여기서 data가 준비되어 있는지, 준비되어 있지 않은지, 다른 예외가 있는지 확인할 수 있는 방법이 필요할 것이다.

	void Server(const string& port)
	{
		cout << "UDP blocking server start" << endl;

		// Udp socket을 만든다.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// 자신의 주소를 등록한다.
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);

		// 현재 socket을 non-blocking mode로 바꾼다.
		//socket->setNoneBlockingMode(true);

		// 통신
		while (1)
		{
			SocketAddress client_address;
			char buffer[1024] = { 0 };
			// 여기서 block 상태에 빠지기 때문에~
			socket->receiveFrom(buffer, 1024, client_address);

			cout << "[server received] address : "
				<< client_address.toString()
				<< "   data : "
				<< buffer << endl;

			string message = "hello! client im server!";
			socket->sendTo(message.c_str(), message.size(), client_address);

			// ~게임 logic을 수행하지 못하게 된다.
			cout << "process in game logic.." << endl;
		}
	}


	void Client(const string& server_addr)
	{
		cout << "UDP blocking client start" << endl;

		// Udp socket을 만든다
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		
		// 자신의 주소 등록(선택)
		SocketAddress address;
		socket->bind(address);
		//socket->setNoneBlockingMode(true);

		// Server 주소를 만든다.
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