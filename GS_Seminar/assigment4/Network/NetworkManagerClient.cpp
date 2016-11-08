#include <iostream>
#include <sstream>
#include "NetworkManagerClient.h"
#include <conio.h>
using namespace std;

namespace
{
	// Server�� client map�� �� client�� id�� �ּ� ������ �߰��Ѵ�.
	const string kLoginPacket = "<Login>";

	// ������ �������� ���, client���� �Ʒ� ���ڿ��� ������.
	const string kOkayPacket = "<Okay>";
}

std::unique_ptr<NetworkManagerClient> NetworkManagerClient::instance = nullptr;


void NetworkManagerClient::staticInit(const string& server_addr)
{
	instance.reset(new NetworkManagerClient());
	instance->init(server_addr);
}

NetworkManagerClient::NetworkManagerClient()
	:
	_socket(UDPSocket::create(SocketUtil::AddressFamily::INET)),
	_address(new SocketAddress())
{}


bool NetworkManagerClient::init(const string& server_addr)
{
	// �ڽ��� �ּ� ���
	_socket->bind(*_address);

	// Server�� �ּҸ� �����Ѵ�.
	_server_address.reset(SocketAddress::createFromString(server_addr));


	// Server�� �α��� ��û�� �Ѵ�.
	_socket->sendTo(kLoginPacket.c_str(), kLoginPacket.size(), *_server_address);

	// Socket�� non-blocking mode�� �ٲ۴�.
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerClient::update()
{
	// Enter�� �Էµ� ������ ���ڸ� �Է¹޴´�.
	// cin�� ���� blocking io�̱� ������ ������� ���Ѵ�.
	if (_kbhit())
	{
		char ch = _getch();
		inputs[inputCur++] = ch;
		cout << ch;

		// Enter �Է� ��
		if (ch == 13)
		{
			inputs[inputCur] = 0;
			_socket->sendTo(inputs, inputCur, *_server_address);
			inputCur = 0;
		}
	}

	// Server�κ��� �����͸� �޴´�.
	SocketAddress temp;
	char buffer[1024] = { 0 };
	int read_bytes = _socket->receiveFrom(buffer, 1024, temp);

	if (read_bytes == 0)
	{
	}
	else if (read_bytes == -WSAECONNRESET)
	{
	}
	else if (read_bytes > 0)
	{
		cout << buffer << endl;
	}
	else
	{
	}
}
