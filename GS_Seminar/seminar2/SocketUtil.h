#pragma once

#include <vector>
#include "UDPSocket.h"
#include "TCPSocket.h"

enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:

	// 윈도우 socket을 사용하기 위한 초기화 작업
	static bool			staticInit();

	// 윈도우 socket 사용 후 정리
	static void			cleanUp();

	static void			reportError( const char* inOperationDesc );
	static int			getLastError();

	static UDPSocketPtr	CreateUDPSocket( SocketAddressFamily inFamily );
	static TCPSocketPtr	CreateTCPSocket( SocketAddressFamily inFamily );
};