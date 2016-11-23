#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManagerLobbyServer.h"
#include "PacketFactory.h"
using namespace std;

std::unique_ptr<NetworkManagerLobbyServer> NetworkManagerLobbyServer::instance = nullptr;

void NetworkManagerLobbyServer::staticInit(uint16_t port)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerLobbyServer(port));
	instance->init();
}

NetworkManagerLobbyServer::NetworkManagerLobbyServer(uint16_t port)
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress(INADDR_ANY, port));
}

bool NetworkManagerLobbyServer::init()
{
	_state = kWaitingRoom;
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerLobbyServer::update()
{
	recv();
	handleQueuedPackets();
}


void NetworkManagerLobbyServer::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
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
	else if (packet.getType() == PacketFactory::kCreateRoom)
	{
		cout << "packet : [CreateRoom]  from : " << from.toString() << endl;
		handleCreateRoomPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		cout << "can't handle this packet : " << packet.getType() << std::endl;
	}
	cout << endl;
}

void NetworkManagerLobbyServer::handleHelloPacket(
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
	
	// Make other users data
	std::vector<std::pair<int, std::string>> users;
	for (const auto& c : _clients)
	{
		int id = c.first;
		users.push_back(std::make_pair(id, _id_to_name[id]));
	}
	// Response to joined client with other users data
	GamePacket& joined = PacketFactory::createJoinedPacket(users, 0, false);
	send(joined, from);

	// Response to all other clients
	GamePacket& intro = PacketFactory::createIntroPacket(client_id, name, 0, false);
	for (auto c : _clients)
		if(c.first != client_id)
			send(intro, c.second);
}

void NetworkManagerLobbyServer::handleMessagePacket(
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

void NetworkManagerLobbyServer::handleDisconnectionPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	auto iter = _address_to_id.find(from);
	int id = iter->second;

	removeClient(from);

	GamePacket& packet = PacketFactory::createNotifyDisconnectedPacket(id, 0, false);

	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerLobbyServer::handleCreateRoomPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{

}

void NetworkManagerLobbyServer::handleRoomHasDestroyedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
}