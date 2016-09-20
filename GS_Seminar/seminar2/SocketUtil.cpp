#include "SocketUtil.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

bool SocketUtil::staticInit()
{
#if _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ( iResult != NO_ERROR )
	{
		reportError ("Starting Up");
		return false;
	}
#endif
	return true;
}

void SocketUtil::cleanUp()
{
#if _WIN32
	WSACleanup();
#endif
}


void SocketUtil::reportError( const char* inOperationDesc )
{
#if _WIN32
	LPVOID lpMsgBuf;
	DWORD errorNum = GetLastError();
	
	FormatMessage(
				  FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  errorNum,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR) &lpMsgBuf,
				  0, NULL );
	
	
	//LOG( "Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf );
#else
	LOG( "Error: %hs", inOperationDesc );
#endif
}

int SocketUtil::getLastError()
{
#if _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

UDPSocketPtr SocketUtil::CreateUDPSocket( SocketAddressFamily inFamily )
{
	SOCKET s = socket( inFamily, SOCK_DGRAM, IPPROTO_UDP );
	
	if( s != INVALID_SOCKET )
	{
		return UDPSocketPtr( new UDPSocket( s ) );
	}
	else
	{
		reportError( "SocketUtil::CreateUDPSocket" );
		return nullptr;
	}
}

TCPSocketPtr SocketUtil::CreateTCPSocket( SocketAddressFamily inFamily )
{
	SOCKET s = socket( inFamily, SOCK_STREAM, IPPROTO_TCP );
	
	if( s != INVALID_SOCKET )
	{
		return TCPSocketPtr( new TCPSocket( s ) );
	}
	else
	{
		reportError( "SocketUtil::CreateTCPSocket" );
		return nullptr;
	}
}
