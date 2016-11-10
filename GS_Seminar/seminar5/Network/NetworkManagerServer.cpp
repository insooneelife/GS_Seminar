#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManagerServer.h"
#include "PacketFactory.h"
using namespace std;

std::unique_ptr<NetworkManagerServer> NetworkManagerServer::instance = nullptr;

void NetworkManagerServer::staticInit(uint16_t port)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerServer(port));
	instance->init();
}

NetworkManagerServer::NetworkManagerServer(uint16_t port)
	:
	_appointed_id(0),
	_time(0)
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress(INADDR_ANY, port));
}

bool NetworkManagerServer::init()
{
	_state = kWaitingRoom;
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerServer::update()
{
	recv();

	if (_state == kWaitingRoom)
	{
	}
	else if (_state == kStarting)
	{
		_time++;
		if (_time > 100)
		{
			// for all send EnterPlayingState
		}
	}
	else if (_state == kPlaying)
	{
	}

	handleQueuedPackets();
}


void NetworkManagerServer::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kHello)
	{
		cout << "packet : [HelloPacket]  from : " << from.toString() << endl;
		handleHelloPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kMessage)
	{
		cout << "packet : [MessagePacket]  from : " << from.toString() << endl;
		handleMessagePacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kDisconnection)
	{
		cout << "packet : [Disconnection]  from : " << from.toString() << endl;
		handleDisconnectionPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		cout << "can't handle this packet : " << packet.getType() << std::endl;
	}
	cout << endl;
}

void NetworkManagerServer::handleHelloPacket(
	const SocketAddress& from,
	const uint8_t* buffer, 
	size_t length)
{
	// Verify
	assert(Data::VerifyUserDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [UserData]!");

	// Process packet logic
	int client_id = _clients.size() + 1;
	auto data = Data::GetUserData(buffer);
	std::string name = data->name()->c_str();

	insertClient(client_id, from, name);
	
	// Appoint new room master
	auto find = _clients.find(_appointed_id);
	bool change = false;
	int appointed_id = 0;
	if (find != end(_clients))
	{
		change = false;
	}
	else
	{
		change = true;
		appointed_id = client_id;
	}

	// Response to all
	GamePacket& packet = PacketFactory::createJoinedPacket(client_id, name, appointed_id, change);
	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerServer::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyMessageDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [MessageData]!");

	// Process packet logic
	auto data = Data::GetMessageData(buffer);
	string name = data->user()->name()->c_str();
	string message = data->msg()->c_str();

	// Response to all
	GamePacket& packet = PacketFactory::createMessagePacket(data->user()->id(), name, message);
	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerServer::handleDisconnectionPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	auto iter = _address_to_id.find(from);
	int id = iter->second;
	removeClient(from);

	int new_appointed_id = 0;
	if (id == _appointed_id)
	{
		if (_clients.size() > 0)
		{
		}
		else
		{
			std::cout << "close this room" << std::endl;
		}
	}

	//GamePacket& packet = PacketFactory::createNotifyDisconnectedPacket(id, new_appointed_id, )
	//for(auto c : _clients)
	//	send()
}

void NetworkManagerServer::handleRequestStartPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	_state = kStarting;
}

void NetworkManagerServer::handleReadyPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	int id = _address_to_id[from];
	_ready_set.insert(id);
	if (_ready_set.size() == _clients.size())
	{
		_state = kPlaying;
	}
}