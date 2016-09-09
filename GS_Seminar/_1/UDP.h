#include <string>
#include <iostream>
#include <Windows.h>
#include <stdio.h>

using namespace std;

const int BufferSize = 256;
const int Port = 9999;
const std::string IP = "127.0.0.1";

namespace
{
	void initWindowSocket()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR)
		{
			cout << "����. error code : " << WSAGetLastError() << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}
	}

	SOCKET createSocket()
	{
		SOCKET sock;
		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			cout << "socket() ���� ����. error code : " << WSAGetLastError() << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		return sock;
	}

	void sendProcess(struct sockaddr_in* otherAddress, SOCKET socket, string message)
	{
		// Message ����
		int ret =
			sendto(
				socket,
				message.c_str(),
				message.size(),
				0,
				(struct sockaddr *) otherAddress,
				sizeof(struct sockaddr_in));

		// ���� �� �۾�
		if (ret == SOCKET_ERROR)
		{
			cout << "sendto() ���� ����  error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}
	}

	void recvProcess(struct sockaddr_in* otherAddress, SOCKET socket)
	{
		// Message ����
		char recvMessage[BufferSize] = { 0 };
		int len = sizeof(struct sockaddr_in);

		int ret =
			recvfrom(
				socket,
				recvMessage,
				BufferSize,
				0,
				(struct sockaddr *) otherAddress,
				&len);

		// ���� �� �۾�
		if (ret == SOCKET_ERROR)
		{
			cout << "recvfrom() ����  error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}

		// ���� ���
		cout << "size : " << ret << "  data : " << recvMessage << endl;
	}

	void UDPClient()
	{
		cout << "UDP Client" << endl;

		// Window socket �ʱ�ȭ
		initWindowSocket();

		// Socket ����
		SOCKET mySocket = createSocket();

		// Address ����ü �ʱ�ȭ
		struct sockaddr_in otherAddress;
		memset((char *)&otherAddress, 0, sizeof(otherAddress));
		otherAddress.sin_family = AF_INET;
		otherAddress.sin_port = htons(Port);
		otherAddress.sin_addr.s_addr = inet_addr(IP.c_str());

		// ��� ����
		while (1)
		{
			cout<<"Message �Է� : ";
			string message;
			cin >> message;

			// Server�� �۽�
			sendProcess(&otherAddress, mySocket, message);
			
			// Server�κ��� ����
			recvProcess(&otherAddress, mySocket);
		}

		closesocket(mySocket);
		WSACleanup();
	}

	void UDPServer()
	{
		cout << "UDP Server" << endl;
	
		// Window socket �ʱ�ȭ
		initWindowSocket();

		// Socket ����
		SOCKET mySocket = createSocket();

		// Address ����ü �ʱ�ȭ
		struct sockaddr_in myAddress;
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = inet_addr(IP.c_str());
		myAddress.sin_port = htons(Port);

		// Socket�� �ּ� �Ҵ�
		if (bind(mySocket, (SOCKADDR*)&myAddress, sizeof(myAddress)) == SOCKET_ERROR)
		{
			std::cout << "�ּ� �Ҵ� ����" << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}

		while (1)
		{
			int len = sizeof(struct sockaddr_in);
			
			// Client�κ��� messge ����
			recvProcess(&myAddress, mySocket);

			// Client�� message �۽�
			sendProcess(&myAddress, mySocket, "SERVER MSG");
		}
	}
}






