#include "TCPSocket.h"
#include "SocketUtil.h"

int TCPSocket::Connect( const SocketAddress& inAddress )
{
	int err = connect( mSocket, &inAddress._sockaddr, inAddress.getSize() );
	if( err < 0 )
	{
		SocketUtil::reportError( "TCPSocket::Connect" );
		return -SocketUtil::getLastError();
	}
	return NO_ERROR;
}

int TCPSocket::Listen( int inBackLog )
{
	int err = listen( mSocket, inBackLog );
	if( err < 0 )
	{
		SocketUtil::reportError( "TCPSocket::Listen" );
		return -SocketUtil::getLastError();
	}
	return NO_ERROR;
}

TCPSocketPtr TCPSocket::Accept( SocketAddress& inFromAddress )
{
	socklen_t length = inFromAddress.getSize();
	SOCKET newSocket = accept( mSocket, &inFromAddress._sockaddr, &length );

	if( newSocket != INVALID_SOCKET )
	{
		return TCPSocketPtr( new TCPSocket( newSocket ) );
	}
	else
	{
		SocketUtil::reportError( "TCPSocket::Accept" );
		return nullptr;
	}
}

int32_t	TCPSocket::Send( const void* inData, size_t inLen )
{
	int bytesSentCount = send( mSocket, static_cast< const char* >( inData ), inLen, 0 );
	if( bytesSentCount < 0 )
	{
		SocketUtil::reportError( "TCPSocket::Send" );
		return -SocketUtil::getLastError();
	}
	return bytesSentCount;
}

int32_t	TCPSocket::Receive( void* inData, size_t inLen )
{
	int bytesReceivedCount = recv( mSocket, static_cast< char* >( inData ), inLen, 0 );
	if( bytesReceivedCount < 0 )
	{
		SocketUtil::reportError( "TCPSocket::Receive" );
		return -SocketUtil::getLastError();
	}
	return bytesReceivedCount;
}

int TCPSocket::Bind( const SocketAddress& inBindAddress )
{
	int error = bind( mSocket, &inBindAddress._sockaddr, inBindAddress.getSize() );
	if( error != 0 )
	{
		SocketUtil::reportError( "TCPSocket::Bind" );
		return SocketUtil::getLastError();
	}

	return NO_ERROR;
}

TCPSocket::~TCPSocket()
{
#if _WIN32
	closesocket( mSocket );
#else
	close( mSocket );
#endif
}
