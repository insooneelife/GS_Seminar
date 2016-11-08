#include <iostream>
#include <sstream>
#include "NetworkManagerClient.h"
#include <conio.h>
using namespace std;

namespace
{
	// Server의 client map에 새 client의 id와 주소 정보를 추가한다.
	const string kLoginPacket = "<Login>";

	// 접속이 성공적일 경우, client에게 아래 문자열을 보낸다.
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
	// 자신의 주소 등록
	_socket->bind(*_address);

	// Server의 주소를 생성한다.
	_server_address.reset(SocketAddress::createFromString(server_addr));


	// Server에 로그인 요청을 한다.
	_socket->sendTo(kLoginPacket.c_str(), kLoginPacket.size(), *_server_address);

	// Socket을 non-blocking mode로 바꾼다.
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerClient::update()
{
	// Enter가 입력될 때까지 문자를 입력받는다.
	// cin의 경우는 blocking io이기 때문에 사용하지 못한다.
	if (_kbhit())
	{
		char ch = _getch();
		inputs[inputCur++] = ch;
		cout << ch;

		// Enter 입력 시
		if (ch == 13)
		{
			inputs[inputCur] = 0;
			_socket->sendTo(inputs, inputCur, *_server_address);
			inputCur = 0;
		}
	}

	// Server로부터 데이터를 받는다.
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
