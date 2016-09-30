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
	// ���� �������� socket�� ���� send/recv ���� ������ ���Ҵ�.
	// �׷��ٸ� ���� ���� ���� client�� ����ϴ� server�� ������ ����.
	// ������ ���� �Ʒ��� ���� ������ �� ���� ������ ����� ���� �� �� ���� ���̴�.
	// �⺻������ accept, send, recv �Լ����� �����ϸ� ���� main thread�� blocking ���¿� ���� �ϱ� ������,
	// server���� �� client���� recv�� �ϰ� ������ �ٸ� client���� ����� ���õ� �� �ۿ� ���� �ȴ�. (accept�� ��������)
	// �׷��� ������ socket�� non-blocking ���·� �ٲپ��־� ������ ���� �ذ��Ѵ�.

	void Server(const string& port)
	{
		cout << "TCP chat server start" << endl;

		// Tcp Socket�� �����. 
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));
		
		// Port ������ �̿��Ͽ� �ּҸ� ����� socket�� ����Ѵ�.
		// Port�� �̿��ϴ� ������,
		// �ڱ� �ڽ��� ip�� socket�� ����ϴ� ��쿡 ip������ ���� ���� �ʿ䰡 ���� �����̴�. 
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		
		// Socket�� ���� ��� ���� �����Ų��.
		// �ִ� ���� �ο��� 32��
		socket->listen(32);

		// Tcp ����� ��� ������ �����ؾ��ϱ� ������,
		// ���� ������ ���������� ������ socket���� ���� �־�� �Ѵ�.
		vector<unique_ptr<TCPSocket>> clients;

		while (1)
		{
			SocketAddress client_address;
			cout << "blocked in accept" << endl;

			// accept�� ���� �� ������ ���������� ������,
			// ������ ��û�� client�� ����ϴ� socket�� ��ȯ�Ͽ� vector�� �ִ´�.
			clients.emplace_back(socket->accept(client_address));

			// ��� vector�� ��ȸ�ϸ鼭 data�� recv �Ѵ�.
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

		// TCP socket�� �����.
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));

		// ������ server�� �ּ�
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(address));
		
		// ������ ��û�Ѵ�.
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
	// Udp socket�� ���, tcp�� �޸� �ϳ��� socket�� ���� ���� client���� data�� �о�� �� �ֱ� ������,
	// blocking ���¿� �������� tcpó�� �ٸ� client���� ����� �Ұ��������� ��Ȳ�� ������ �ʴ´�.
	// ������ socket�� �ϳ�����, recv�� ���� blocking ���¿� ������ ���� server�� �۾��� ������ �� ���� �ȴ�.
	// �׷��Ƿ� �ᱹ udp socket�� blocking mode�δ� ������ �����ϱⰡ �����.

	// Socket�� default�� blocking ���� �����Ǿ� �ֱ� ������ udp�� socket�� blocking socket�̴�.
	// ioctlsocket �Լ��� ���� socket�� non-blocking ���� �ٲ� �� �ִµ�,
	// �� ������ UDPsocket class ���ο� setNoneBlockingMode �Լ��� ���� ������ �ξ���.

	// Socket�� non-blocking ��忡 ���� �Ǹ� �� �̻� send/recv�� �ص� blocking ���¿� ���� �ʴ´�.
	// Data�� �غ�Ǿ� ������ ��������, �غ�Ǿ� ���� �ʴٸ� �ٷ� �Ѿ��.
	// �׷��ٸ� ���⼭ data�� �غ�Ǿ� �ִ���, �غ�Ǿ� ���� ������, �ٸ� ���ܰ� �ִ��� Ȯ���� �� �ִ� ����� �ʿ��� ���̴�.

	void Server(const string& port)
	{
		cout << "UDP blocking server start" << endl;

		// Udp socket�� �����.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// �ڽ��� �ּҸ� ����Ѵ�.
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);

		// ���� socket�� non-blocking mode�� �ٲ۴�.
		//socket->setNoneBlockingMode(true);

		// ���
		while (1)
		{
			SocketAddress client_address;
			char buffer[1024] = { 0 };
			// ���⼭ block ���¿� ������ ������~
			socket->receiveFrom(buffer, 1024, client_address);

			cout << "[server received] address : "
				<< client_address.toString()
				<< "   data : "
				<< buffer << endl;

			string message = "hello! client im server!";
			socket->sendTo(message.c_str(), message.size(), client_address);

			// ~���� logic�� �������� ���ϰ� �ȴ�.
			cout << "process in game logic.." << endl;
		}
	}


	void Client(const string& server_addr)
	{
		cout << "UDP blocking client start" << endl;

		// Udp socket�� �����
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		
		// �ڽ��� �ּ� ���(����)
		SocketAddress address;
		socket->bind(address);
		//socket->setNoneBlockingMode(true);

		// Server �ּҸ� �����.
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
	// winsock �ʱ�ȭ
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