#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"

class NetworkManagerClient
{
public:
	static std::unique_ptr<NetworkManagerClient> instance;
	static void staticInit(const std::string& server_addr);

	NetworkManagerClient();
	~NetworkManagerClient()
	{}

	bool init(const std::string& server_addr);
	void update();

private:

	std::unique_ptr<UDPSocket> _socket;
	std::unique_ptr<SocketAddress> _address;
	std::unique_ptr<SocketAddress> _server_address;

	// Keyboard 입력을 담을 buffer
	char inputs[128] = { 0 };
	char inputCur = 0;
};

