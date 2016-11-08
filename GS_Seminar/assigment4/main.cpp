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

	// pos�� ���� packet �����Լ�
	// Monster�� ���ؼ��� ���� �� ���� ���̴�.
	GamePacket createPosPacket(float x, float y, float z)
	{
		// ����ȭ
		flatbuffers::FlatBufferBuilder builder;
		auto pos_data = MyGame::Createpos(builder, x, y, z);
		builder.Finish(pos_data);

		// Type������ Size������ pakcet header�� ����ϱ� ���� GameMessage�� ���Ѵ�.
		GamePacket packet(builder, Types::kPos);
		return packet;

		// GameMessage�� body���� flatbuffers�� ����ȭ�� data�� ����ȴ�.
		// packet.getBody();

		// flatbuffers data�� ����
		// packet.getBodyLength();

		// GameMessage�� data���� flatbuffers data + header data�� ����ȴ�.
		// header data���� ���� ������, Ÿ�� ������ ���Եȴ�.
		// packet.getData();

		// flatbuffers data + header data�� ����
		// packet.getLength();

		// Ÿ���� ���������� Ȯ���ϴ� �Լ�
		// packet.getType();
	}

	void handlePosPacket(const uint8_t* body, size_t body_length)
	{
		// ����
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// ������ȭ
		auto pos = MyGame::Getpos(body);

		// ���
		cout << pos->x() << ", " << pos->y() << ", " << pos->z() << endl;
	}

	void handleMonsterPacket(const uint8_t* body, size_t body_length)
	{
		// ����
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// ������ȭ
		auto monster = MyGame::GetMonster(body);

		// ���
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

		// pos�� ���� packet�� �����.
		GamePacket& send_packet = createPosPacket(x, y, z);

		// ��Ʈ��ũ�� ���� getData()�� data�� send�� �� ���� ���̴�.
		// buffer�� ���� ������ send�� �ȴٰ� ��������.
		uint8_t buffer[4000] = { 0 };
		size_t length = 0;

		memcpy(buffer, send_packet.getData(), send_packet.getLength());
		length = send_packet.getLength();


		// send(send_packet.getData(), send_packet.getLength())
		// ..
		// ��Ʈ��ũ�� ���� �ٸ� ��ǻ�Ϳ��� �����͸� recv�Ѵٰ� �����Ѵ�.
		// �����ʹ� recv�� ���� buffer�� ����ȴٰ� �����Ѵ�.
		// ..
		// recv(buffer)

		
		// ���� buffer���� � �����Ͱ� �ִ��� �� �� ������,
		// buffer�� length�� GameMessage�� ������Ų�ٸ�,
		GamePacket recv_packet(buffer, length);

		// Type�� �о�� �� �ִ�.
		if (recv_packet.getType() == Types::kPos)
		{
			// Type�� �°� body�� �ִ� flatbuffer�� ����ȭ �� data�� ó�����ش�.
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

