#include "TCPSocket.h"
#include <iostream>

TCPSocket* TCPSocket::create(SocketUtil::AddressFamily family)
{
	SOCKET s = socket(family, SOCK_STREAM, IPPROTO_TCP);

	if (s != INVALID_SOCKET)
	{
		return new TCPSocket(s);
	}
	else
	{
		std::cout << "TCPSocket::create" << std::endl;
		SocketUtil::reportError("TCPSocket::create");
		return nullptr;
	}
}

int TCPSocket::connect(const SocketAddress& address)
{
	int err = ::connect(_socket, &address._sockaddr, address.getSize());
	if(err < 0)
	{
		std::cout << "TCPSocket::connect" << std::endl;
		SocketUtil::reportError("TCPSocket::connect");
		return -SocketUtil::getLastError();
	}
	return NO_ERROR;
}

int TCPSocket::listen(int back_num)
{
	int err = ::listen(_socket, back_num);
	if(err < 0)
	{
		std::cout << "TCPSocket::listen" << std::endl;
		SocketUtil::reportError("TCPSocket::listen");
		return -SocketUtil::getLastError();
	}
	return NO_ERROR;
}

TCPSocket* TCPSocket::accept(SocketAddress& from_addr)
{
	socklen_t length = from_addr.getSize();
	SOCKET new_socket = ::accept(_socket, &from_addr._sockaddr, &length);

	if(new_socket != INVALID_SOCKET)
	{
		return new TCPSocket(new_socket);
	}
	else
	{
		std::cout << "TCPSocket::accept" << std::endl;
		SocketUtil::reportError("TCPSocket::accept");
		return nullptr;
	}
}

int32_t	TCPSocket::send(const void* data, size_t length)
{
	int bytes_sent = ::send(_socket, static_cast<const char*>(data), length, 0);
	if (bytes_sent < 0)
	{
		std::cout << "TCPSocket::send" << std::endl;
		SocketUtil::reportError("TCPSocket::send");
		return -SocketUtil::getLastError();
	}
	return bytes_sent;
}

int32_t	TCPSocket::receive(void* data, size_t length)
{
	int bytes_received = ::recv(_socket, static_cast<char*>(data), length, 0);
	if (bytes_received < 0)
	{
		int error = -SocketUtil::getLastError();
		std::cout << bytes_received << "  TCPSocket::receive" <<" "<< error << std::endl;
		SocketUtil::reportError("TCPSocket::receive");
		return error;
	}
	return bytes_received;
}

int TCPSocket::bind(const SocketAddress& address)
{
	int error = ::bind(_socket, &address._sockaddr, address.getSize());
	if (error != 0)
	{
		std::cout << "TCPSocket::bind" << std::endl;
		SocketUtil::reportError("TCPSocket::bind");
		return SocketUtil::getLastError();
	}

	return NO_ERROR;
}

TCPSocket::~TCPSocket()
{
#if _WIN32
	closesocket(_socket);
#else
	close( mSocket );
#endif
}
