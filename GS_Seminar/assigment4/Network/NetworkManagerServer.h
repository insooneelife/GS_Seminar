#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"

class NetworkManagerServer
{
public:
	static std::unique_ptr<NetworkManagerServer> instance;
	static void staticInit(uint16_t port);

	NetworkManagerServer(uint16_t port);
	~NetworkManagerServer() {};

	bool init();
	void update();

private:

	std::unique_ptr<UDPSocket> _socket;
	std::unique_ptr<SocketAddress> _address;

	std::unordered_map<int, SocketAddress> clients;
	std::unordered_map<SocketAddress, int> addressToID;
};

