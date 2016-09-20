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

	// ������ socket�� ����ϱ� ���� �ʱ�ȭ �۾�
	static bool			staticInit();

	// ������ socket ��� �� ����
	static void			cleanUp();

	static void			reportError( const char* inOperationDesc );
	static int			getLastError();

	static UDPSocketPtr	CreateUDPSocket( SocketAddressFamily inFamily );
	static TCPSocketPtr	CreateTCPSocket( SocketAddressFamily inFamily );
};