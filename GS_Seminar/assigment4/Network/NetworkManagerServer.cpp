#include <iostream>
#include <sstream>
#include "NetworkManagerServer.h"
using namespace std;

namespace
{
	// Server�� client map�� �� client�� id�� �ּ� ������ �߰��Ѵ�.
	const string kLoginPacket = "<Login>";

	// ������ �������� ���, client���� �Ʒ� ���ڿ��� ������.
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
	// Client�� �ּҸ� ���� �ӽ� ����
	SocketAddress client_address;

	// Client�κ��� ���� �����͸� ���� ����
	char buffer[1024] = { 0 };

	// Non-blocking mode�� socket������ recv�Լ��� return���� ����
	// ���� � ��Ȳ���� �� �� �ִ�.
	int read_bytes = _socket->receiveFrom(buffer, 1024, client_address);

	// ���� ���� ������ �񵿱� ��忡 �°� receiveFrom���� block���� �ʰ� �Ѿ��.
	if (read_bytes == 0)
	{
		// nothing to read
	}
	// Client���� socket�� �������� �˸��� ���̴�.
	// �� ���� return ������ ��ȯ�Ǳ� ���ؼ���,
	// server �ʿ��� sendTo �Լ��� ���� �� client���� ���𰡸� ������ �Ѵ�.
	else if (read_bytes == -WSAECONNRESET)
	{
		cout << "������ ���� client : " << client_address.toString() << endl;

		auto iter = addressToID.find(client_address);
		int id = iter->second;
		clients.erase(id);
		addressToID.erase(client_address);
	}
	// ���� data�� ���� ��� return ������ data�� ũ�Ⱑ ��ȯ�ȴ�.
	else if (read_bytes > 0)
	{
		cout << "[server received] address : "
			<< client_address.toString()
			<< "   data : "
			<< buffer << endl;

		// ���� packet�� ���� ó��
		string data = buffer;
		if (data == kLoginPacket)
		{
			// "<Login>" ������ ���ڿ��� ���� ���, �� client�� clients hashmap�� �߰��Ѵ�.
			int client_num = clients.size();
			clients.emplace(client_num + 1, client_address);
			addressToID.emplace(client_address, client_num + 1);

			// ���������� ���ӵǾ����� client���� �˸���.
			_socket->sendTo(kOkayPacket.c_str(), kOkayPacket.size(), client_address);
		}
		else
		{
			auto iter = addressToID.find(client_address);
			int id = iter->second;

			stringstream ss;
			ss << id;
			data = ss.str() + " : " + data;

			// ��� client���� ���� packet(data)�� �����Ѵ�.
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
