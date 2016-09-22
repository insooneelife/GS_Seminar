#include "UDPSocket.h"

UDPSocket* UDPSocket::create(SocketUtil::AddressFamily family)
{
	SOCKET s = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);

	if (s != INVALID_SOCKET)
	{
		return new UDPSocket(s);
	}
	else
	{
		SocketUtil::reportError("UDPSocket::create");
		return nullptr;
	}
}


int UDPSocket::bind(const SocketAddress& to_addr)
{
	int error = ::bind(_socket, &to_addr._sockaddr, to_addr.getSize());
	if (error != 0)
	{
		SocketUtil::reportError("UDPSocket::bind");
		return SocketUtil::getLastError();
	}
	
	return NO_ERROR;
}


int UDPSocket::sendTo(const void* data, int length, const SocketAddress& to_addr)
{
	int byteSentCount = ::sendto(_socket, static_cast<const char*>(data), length, 0, &to_addr._sockaddr, to_addr.getSize());
	if(byteSentCount <= 0)
	{
		//we'll return error as negative number to indicate less than requested amount of bytes sent...
		SocketUtil::reportError("UDPSocket::sendTo");
		return -SocketUtil::getLastError();
	}
	else
	{
		return byteSentCount;
	}
}


int UDPSocket::receiveFrom(void* data, int max_length, SocketAddress& from_addr)
{
	socklen_t fromLength = from_addr.getSize();
	
	int readByteCount = ::recvfrom(_socket, static_cast<char*>(data), max_length, 0, &from_addr._sockaddr, &fromLength);
	if (readByteCount >= 0)
	{
		return readByteCount;
	}
	else
	{
		int error = SocketUtil::getLastError();
		if (error == WSAEWOULDBLOCK)
		{
			return 0;
		}
		else if (error == WSAECONNRESET)
		{
			//this can happen if a client closed and we haven't DC'd yet.
			//this is the ICMP message being sent back saying the port on that computer is closed
			//LOG( "Connection reset from %s", outFromAddress.ToString().c_str() );
			return -WSAECONNRESET;
		}
		else
		{
			SocketUtil::reportError("UDPSocket::ReceiveFrom");
			return -error;
		}
	}
}

UDPSocket::~UDPSocket()
{
#if _WIN32
	closesocket(_socket);
#else
	close( mSocket );
#endif
}


int UDPSocket::setNoneBlockingMode(bool flag)
{
#if _WIN32
	u_long arg = flag ? 1 : 0;
	int result = ioctlsocket(_socket, FIONBIO, &arg);
#else
	int flags = fcntl( mSocket, F_GETFL, 0 );
	flags = inShouldBeNonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK);
	int result = fcntl( mSocket, F_SETFL, flags );
#endif
	
	if( result == SOCKET_ERROR )
	{
		SocketUtil::reportError("UDPSocket::SetNonBlockingMode");
		return SocketUtil::getLastError();
	}
	else
	{
		return NO_ERROR;
	}
}

