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
	else if (packet.getType() == PacketFactory::kRoomIntro)
	{
		cout << "packet : [RoomIntro]  from : " << from.toString() << endl;
		handleRoomIntroPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kJoinRoom)
	{
		cout << "packet : [JoinRoom]  from : " << from.toString() << endl;
		handleJoinRoomPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kRequestShowRoomInfo)
	{
		cout << "packet : [RequestShowRoomInfo]  from : " << from.toString() << endl;
		handleRequestShowRoomInfoPacket(from, packet.getBody(), packet.getBodyLength());
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
	
	// Response to all other clients
	GamePacket& intro = PacketFactory::createIntroPacket(client_id, name, 0, false);
	for (auto c : _clients)
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
	auto iter = _clients_addr_to_id.find(from);
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
	// Verify
	assert(Data::VerifyUserDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [UserData]!");

	// Process packet logic
	auto data = Data::GetUserData(buffer);
	int id = data->id();
	string name = data->name()->c_str();

	int number = genUniqueID(_rooms);
	_room_masters.emplace(number, std::make_pair(id, from));

	stringstream ss;
	ss << number;

	string filename = "GS_Seminar.exe Room " + ss.str() + " " + _address->toString();
	createRoom(number, filename);
}

void NetworkManagerLobbyServer::handleRoomIntroPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyRoomDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomData]!");

	auto data = Data::GetRoomData(buffer);
	int number = data->number();
	std::string address = from.toString();

	insertRoom(from, number, "");

	auto iter = _room_masters.find(number);
	if (iter != std::end(_room_masters))
	{
		// Send that room is created to client who requested to create room.
		GamePacket& packet = PacketFactory::createRoomIsCreatedPacket(number, address);
		send(packet, iter->second.second);
	}
}

void NetworkManagerLobbyServer::handleJoinRoomPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyRoomDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomData]!");

	auto data = Data::GetRoomData(buffer);
	int number = data->number();

	auto iter = _rooms.find(number);
	if (iter != std::end(_rooms))
	{
		// Send that room is created to client who requested to create room.
		GamePacket& packet = PacketFactory::createRoomIsCreatedPacket(number, iter->second.toString());
		send(packet, from);
		removeClient(iter->second);
	}
}

void NetworkManagerLobbyServer::handleRoomHasDestroyedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
}

void NetworkManagerLobbyServer::handleRequestShowRoomInfoPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	std::vector<std::pair<int, std::string>> rooms;
	for (auto e : _rooms)
	{
		rooms.push_back(std::make_pair(e.first, e.second.toString()));
	}

	GamePacket& packet = PacketFactory::createRoomInfoPacket(rooms);

	for (auto c : _clients)
		send(packet, c.second);
}