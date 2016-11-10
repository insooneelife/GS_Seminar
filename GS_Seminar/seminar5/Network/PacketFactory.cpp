#include "PacketFactory.h"

GamePacket PacketFactory::createPacket(PacketType type)
{
	return GamePacket(type);
}

GamePacket PacketFactory::createHelloPacket(int id, const std::string& name)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	builder.Finish(user_data);

	return GamePacket(builder, kHello);
}

GamePacket PacketFactory::createJoinedPacket(
	int id, const std::string& name,
	int appointedID, bool changed)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	auto appointed_data = Data::CreateAppointedData(builder, appointedID, changed);
	auto joined_data = Data::CreateJoinedData(builder, user_data, appointed_data);
	builder.Finish(joined_data);

	return GamePacket(builder, kJoined);
}

GamePacket PacketFactory::createMessagePacket(
	int id, const std::string& name,
	const std::string& message)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	auto fmessage = builder.CreateString(message);
	auto message_data = Data::CreateMessageData(builder, user_data, fmessage);
	builder.Finish(message_data);

	return GamePacket(builder, kMessage);
}

GamePacket PacketFactory::createNotifyDisconnectedPacket(
	int disconnectdID,
	int appointedID, bool changed)
{
	flatbuffers::FlatBufferBuilder builder;
	auto appointed_data = Data::CreateAppointedData(builder, appointedID, changed);
	auto disconnected_data = Data::CreateDisconnectedData(builder, appointed_data, disconnectdID);
	builder.Finish(disconnected_data);

	return GamePacket(builder, kNotifyDisconnected);
}

