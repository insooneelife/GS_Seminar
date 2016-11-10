#include <iostream>
#include <sstream>
#include <cassert>
#include <conio.h>
#include "NetworkManagerClient.h"
#include "PacketFactory.h"

using namespace std;

std::unique_ptr<NetworkManagerClient> NetworkManagerClient::instance = nullptr;

void NetworkManagerClient::staticInit(const string& server_addr, const std::string& client_name)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerClient(client_name));
	instance->init(server_addr);
}

NetworkManagerClient::NetworkManagerClient(const std::string& client_name)
	:
	_id(0),
	_client_name(client_name),
	_input_message("")
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress());
}


bool NetworkManagerClient::init(const string& server_addr)
{
	_state = kLobby;

	// 자신의 주소 등록
	_socket->bind(*_address);

	// Server의 주소를 생성한다.
	_server_address.reset(SocketAddress::createFromString(server_addr));

	GamePacket& packet = PacketFactory::createHelloPacket(_id, _client_name);
	send(packet, *_server_address);

	return true;
}


void NetworkManagerClient::update()
{
	recv();

	if (_kbhit())
	{
		char ch = _getch();
		cout << ch;

		// Enter 입력 시
		if (ch == 13)
		{
			GamePacket& packet = PacketFactory::createMessagePacket(_id, _client_name, _input_message);
			send(packet, *_server_address);

			_input_message = "";
		}
		else
		{
			_input_message += ch;
		}
	}

	handleQueuedPackets();
}

void NetworkManagerClient::handlePacketByType(
	const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kJoined)
	{
		cout << "packet : [JoinedPacket]  from : " << from.toString() << endl;
		handleJoinedPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kMessage)
	{
		cout << "packet : [MessagePacket]  from : " << from.toString() << endl;
		handleMessagePacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kNotifyDisconnected)
	{
		cout << "packet : [NotifyDisconnectedPacket]  from : " << from.toString() << endl;
		handleNotifyDisconnectedPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kEnterPlaying)
	{
		cout << "packet : [EnterPlayingPacket]  from : " << from.toString() << endl;
		handleEnterPlayingPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		std::cout << "can't handle this packet : " << packet.getType() << std::endl;;
	}
	cout << endl;
}

void NetworkManagerClient::handleJoinedPacket(
	const SocketAddress& from,
	const uint8_t* buffer, 
	size_t length)
{
	// Verify
	assert(Data::VerifyJoinedDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [JoinedData]!");

	// Process packet logic
	_socket->setNoneBlockingMode(true);

	auto data = Data::GetJoinedData(buffer);
	
	if (_state == kLobby)
	{
		_state = kWaitingRoom;
		_id = data->user()->id();
	}
	else
	{}
}

void NetworkManagerClient::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyMessageDataBuffer(flatbuffers::Verifier(buffer, length)) &&
	"Verify failed [MessageData]!");

	// Process packet logic
	auto data = Data::GetMessageData(buffer);

	cout  << "name : " << data->user()->name()->c_str() << "    message : " << data->msg()->c_str() << endl;
}

void NetworkManagerClient::handleNotifyDisconnectedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	std::cout << "handleNotifyDisconnectedPacket" << std::endl;
}

void NetworkManagerClient::handleEnterPlayingPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	_state = kPlaying;
}


