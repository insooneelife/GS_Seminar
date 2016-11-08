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
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress(INADDR_ANY, port));
}

bool NetworkManagerServer::init()
{
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerServer::update()
{
	recv();

	// process other logics ..

	handleQueuedPackets();
}


void NetworkManagerServer::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kLogin)
	{
		cout << "packet : [LoginPacket]  from : " << from.toString() << endl;
		handleLoginPacket(from, packet.getBody(), packet.getBodyLength());
		
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

void NetworkManagerServer::handleLoginPacket(
	const SocketAddress& from,
	const uint8_t* buffer, 
	size_t length)
{
	// Verify
	assert(Packets::VerifyLoginPacketBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [LoginPacket]!");

	// Process packet logic
	int client_id = _clients.size() + 1;
	auto data = Packets::GetLoginPacket(buffer);
	std::string name = data->name()->c_str();

	insertClient(client_id, from, name);
	
	// Response
	GamePacket& packet = PacketFactory::createOkayPacket(client_id);
	send(packet, from);
}

void NetworkManagerServer::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Packets::VerifyMessagePacketBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [MessagePacket]!");

	// Process packet logic
	auto data = Packets::GetMessagePacket(buffer);
	string name = data->name()->c_str();
	string message = data->message()->c_str();

	// Response to all
	GamePacket& packet = PacketFactory::createMessagePacket(name, message);
	for (auto c : _clients)
	{
		send(packet, c.second);
	}
}

void NetworkManagerServer::handleDisconnectionPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	removeClient(from);
}