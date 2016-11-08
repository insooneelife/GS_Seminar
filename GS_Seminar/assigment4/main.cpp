#include <iostream>
#include <bitset>
#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>

#include "flatbuffers/GamePacket.hpp"
#include "flatbuffers/simple_data_generated.h"
#include "flatbuffers/data1_generated.h"
#include "flatbuffers/test_generated.h"

namespace _5_GamePacket
{
	using namespace std;
	enum Types
	{
		kMonster,
		kPos
	};

	// pos에 대한 packet 생성함수
	// Monster에 대해서도 만들 수 있을 것이다.
	GamePacket createPosPacket(float x, float y, float z)
	{
		// 직렬화
		flatbuffers::FlatBufferBuilder builder;
		auto pos_data = MyGame::Createpos(builder, x, y, z);
		builder.Finish(pos_data);

		// Type정보와 Size정보를 pakcet header에 기록하기 위해 GameMessage로 감싼다.
		GamePacket packet(builder, Types::kPos);
		return packet;

		// GameMessage의 body에는 flatbuffers로 직렬화된 data가 저장된다.
		// packet.getBody();

		// flatbuffers data의 길이
		// packet.getBodyLength();

		// GameMessage의 data에는 flatbuffers data + header data가 저장된다.
		// header data에는 길이 정보와, 타입 정보가 포함된다.
		// packet.getData();

		// flatbuffers data + header data의 길이
		// packet.getLength();

		// 타입이 무엇인지만 확인하는 함수
		// packet.getType();
	}

	void handlePosPacket(const uint8_t* body, size_t body_length)
	{
		// 검증
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// 역직렬화
		auto pos = MyGame::Getpos(body);

		// 출력
		cout << pos->x() << ", " << pos->y() << ", " << pos->z() << endl;
	}

	void handleMonsterPacket(const uint8_t* body, size_t body_length)
	{
		// 검증
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// 역직렬화
		auto monster = MyGame::GetMonster(body);

		// 출력
		string name = monster->name()->c_str();
		float x = monster->x();
		float y = monster->y();
		float z = monster->z();
		short mana = monster->mana();
		short hp = monster->hp();
		auto weapons = monster->weapons();

		for (auto w = weapons->begin(); w != weapons->end(); ++w)
		{
			std::cout << w->name()->c_str() << " " << w->damage() << endl;
		}
		std::cout << name << endl;
		std::cout << "pos : " << x << " " << y << " " << z << endl;
		std::cout << "hp/mp : " << hp << " " << mana << endl;
	}
	

	void example()
	{
		std::cout << "_5_GameMessage" << std::endl << std::endl;

		// Server
		float x = 1441.5f;
		float y = -554.7f;
		float z = 994.88f;

		// pos에 대한 packet을 만든다.
		GamePacket& send_packet = createPosPacket(x, y, z);

		// 네트워크를 통해 getData()의 data를 send할 수 있을 것이다.
		// buffer를 통해 실제로 send가 된다고 가정하자.
		uint8_t buffer[4000] = { 0 };
		size_t length = 0;

		memcpy(buffer, send_packet.getData(), send_packet.getLength());
		length = send_packet.getLength();


		// send(send_packet.getData(), send_packet.getLength())
		// ..
		// 네트워크를 통해 다른 컴퓨터에서 데이터를 recv한다고 가정한다.
		// 데이터는 recv를 통해 buffer에 저장된다고 가정한다.
		// ..
		// recv(buffer)

		
		// 현재 buffer에는 어떤 데이터가 있는지 알 수 없지만,
		// buffer와 length로 GameMessage를 생성시킨다면,
		GamePacket recv_packet(buffer, length);

		// Type을 읽어올 수 있다.
		if (recv_packet.getType() == Types::kPos)
		{
			// Type에 맞게 body에 있는 flatbuffer로 직렬화 된 data를 처리해준다.
			handlePosPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
		else if (recv_packet.getType() == Types::kMonster)
		{
			handleMonsterPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
	}
}

#include "Network/NetworkManagerServer.h"
#include "Network/NetworkManagerClient.h"

int main()
{
	SocketUtil::staticInit();
	
	NetworkManagerServer::staticInit(8000);
	while (1)
	{
		NetworkManagerServer::instance->update();
	}

	/*NetworkManagerClient::staticInit("127.0.0.1:8000");
	while (1)
	{
		NetworkManagerClient::instance->update();
	}*/


	SocketUtil::cleanUp();

	
	//_5_GamePacket::example();
	return 0;
}

