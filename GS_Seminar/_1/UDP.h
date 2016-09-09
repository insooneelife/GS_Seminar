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
			cout << "실패. error code : " << WSAGetLastError() << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}
	}

	SOCKET createSocket()
	{
		SOCKET sock;
		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			cout << "socket() 생성 실패. error code : " << WSAGetLastError() << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		return sock;
	}

	void sendProcess(struct sockaddr_in* otherAddress, SOCKET socket, string message)
	{
		// Message 전송
		int ret =
			sendto(
				socket,
				message.c_str(),
				message.size(),
				0,
				(struct sockaddr *) otherAddress,
				sizeof(struct sockaddr_in));

		// 오류 시 작업
		if (ret == SOCKET_ERROR)
		{
			cout << "sendto() 전속 실패  error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}
	}

	void recvProcess(struct sockaddr_in* otherAddress, SOCKET socket)
	{
		// Message 수신
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

		// 오류 시 작업
		if (ret == SOCKET_ERROR)
		{
			cout << "recvfrom() 실패  error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}

		// 응답 출력
		cout << "size : " << ret << "  data : " << recvMessage << endl;
	}

	void UDPClient()
	{
		cout << "UDP Client" << endl;

		// Window socket 초기화
		initWindowSocket();

		// Socket 생성
		SOCKET mySocket = createSocket();

		// Address 구조체 초기화
		struct sockaddr_in otherAddress;
		memset((char *)&otherAddress, 0, sizeof(otherAddress));
		otherAddress.sin_family = AF_INET;
		otherAddress.sin_port = htons(Port);
		otherAddress.sin_addr.s_addr = inet_addr(IP.c_str());

		// 통신 시작
		while (1)
		{
			cout<<"Message 입력 : ";
			string message;
			cin >> message;

			// Server로 송신
			sendProcess(&otherAddress, mySocket, message);
			
			// Server로부터 수신
			recvProcess(&otherAddress, mySocket);
		}

		closesocket(mySocket);
		WSACleanup();
	}

	void UDPServer()
	{
		cout << "UDP Server" << endl;
	
		// Window socket 초기화
		initWindowSocket();

		// Socket 생성
		SOCKET mySocket = createSocket();

		// Address 구조체 초기화
		struct sockaddr_in myAddress;
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = inet_addr(IP.c_str());
		myAddress.sin_port = htons(Port);

		// Socket에 주소 할당
		if (bind(mySocket, (SOCKADDR*)&myAddress, sizeof(myAddress)) == SOCKET_ERROR)
		{
			std::cout << "주소 할당 실패" << endl;
			WSACleanup();
			exit(EXIT_FAILURE);
		}

		while (1)
		{
			int len = sizeof(struct sockaddr_in);
			
			// Client로부터 messge 수신
			recvProcess(&myAddress, mySocket);

			// Client로 message 송신
			sendProcess(&myAddress, mySocket, "SERVER MSG");
		}
	}
}






