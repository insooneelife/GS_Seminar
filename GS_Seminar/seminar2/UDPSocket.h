#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include "SocketAddress.h"
#include "SocketUtil.h"

class UDPSocket
{
public:
	// UDP socketÀÇ »ý¼º
	static UDPSocket* create(SocketUtil::AddressFamily family);
	~UDPSocket();

	int bind(const SocketAddress& to_addr);
	int sendTo(const void* data, int length, const SocketAddress& to_addr);
	int receiveFrom(void* data, int max_length, SocketAddress& from_addr);
	int setNoneBlockingMode(bool flag);

private:
	UDPSocket(SOCKET socket) : _socket(socket) {}
	SOCKET _socket;

};

