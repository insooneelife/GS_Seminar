#include <iostream>
#include "UDP.h"
using namespace std;

int main(int argc, char * argv[])
{
	if (argc == 2)
	{
		string str = argv[1];
		if (str == "Client")
		{
			_ex1::UDPClient();
			Sleep(1000);
			return 0;
		}
		else if (str == "Server")
		{
			_ex1::UDPServer();
			Sleep(1000);
			return 0;
		}
	}

	std::cout << "input argument = Client or Server";

	return 0;
}
