#include <iostream>
#include <sstream>
#include "NetworkManagerServer.h"
using namespace std;

namespace
{
	// Server의 client map에 새 client의 id와 주소 정보를 추가한다.
	const string kLoginPacket = "<Login>";

	// 접속이 성공적일 경우, client에게 아래 문자열을 보낸다.
	const string kOkayPacket = "<Okay>";
}

std::unique_ptr<NetworkManagerServer> NetworkManagerServer::instance = nullptr;


void NetworkManagerServer::staticInit(uint16_t port)
{
	instance.reset(new NetworkManagerServer(port));
	instance->init();
}

NetworkManagerServer::NetworkManagerServer(uint16_t port)
	:
	_socket(UDPSocket::create(SocketUtil::AddressFamily::INET)),
	_address(new SocketAddress(INADDR_ANY, port))
{}

bool NetworkManagerServer::init()
{
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerServer::update()
{
	// Client의 주소를 담을 임시 공간
	SocketAddress client_address;

	// Client로부터 받은 데이터를 담을 버퍼
	char buffer[1024] = { 0 };

	// Non-blocking mode의 socket에서는 recv함수의 return값을 보고
	// 현재 어떤 상황인지 알 수 있다.
	int read_bytes = _socket->receiveFrom(buffer, 1024, client_address);

	// 읽을 것이 없었고 비동기 모드에 맞게 receiveFrom에서 block되지 않고 넘어간다.
	if (read_bytes == 0)
	{
		// nothing to read
	}
	// Client에서 socket이 닫혔음을 알리는 값이다.
	// 이 값이 return 값으로 반환되기 위해서는,
	// server 쪽에서 sendTo 함수를 통해 이 client에게 무언가를 보내야 한다.
	else if (read_bytes == -WSAECONNRESET)
	{
		cout << "연결이 끊긴 client : " << client_address.toString() << endl;

		auto iter = addressToID.find(client_address);
		int id = iter->second;
		clients.erase(id);
		addressToID.erase(client_address);
	}
	// 읽을 data가 있을 경우 return 값으로 data의 크기가 반환된다.
	else if (read_bytes > 0)
	{
		cout << "[server received] address : "
			<< client_address.toString()
			<< "   data : "
			<< buffer << endl;

		// 받은 packet에 대한 처리
		string data = buffer;
		if (data == kLoginPacket)
		{
			// "<Login>" 형태의 문자열을 받을 경우, 이 client를 clients hashmap에 추가한다.
			int client_num = clients.size();
			clients.emplace(client_num + 1, client_address);
			addressToID.emplace(client_address, client_num + 1);

			// 성공적으로 접속되었음을 client에게 알린다.
			_socket->sendTo(kOkayPacket.c_str(), kOkayPacket.size(), client_address);
		}
		else
		{
			auto iter = addressToID.find(client_address);
			int id = iter->second;

			stringstream ss;
			ss << id;
			data = ss.str() + " : " + data;

			// 모든 client에게 받은 packet(data)를 전달한다.
			for (auto c : clients)
			{
				_socket->sendTo(data.c_str(), data.size(), c.second);
			}
		}
	}
	else
	{
		// uhoh, error? exit or just keep going?
	}

	// process other logics ..
}
