#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <iostream>
#include <iomanip>

#include "flatbuffers\flatbuffers.h"

class GameMessage
{
public:
	// header�� ��� ����
	// HEADER_LENGTH �ڸ������� ���
	enum { HEADER_LENGTH = 8 };

	// header�� �� 4�ڸ�, body�� ���̸� ����
	enum { SIZE_HEADER_LENGTH = 4 };

	// header�� �� 4�ڸ�, body�� Ÿ���� ����
	enum { TYPE_HEADER_LENGTH = 4 };

	// body�� ��� ����
	// body�� flatbuffer�� ȣȯ ������ buffer�� ���
	enum { MAX_BODY_LENGTH = 2048 };

	GameMessage()
		:
		_body_length(0),
		_type(0)
	{}

	explicit GameMessage(int type)
	{
		setBodyLength(0);
		setType(type);
		encodeHeader();
	}

	// ������ GameMessage�� ���� ������� data�� (header data + flatbuffer data)
	// �� data�� ���̸� �̿��� ������.
	GameMessage(const uint8_t* data_with_header, size_t length)
	{
		std::memcpy(getData(), data_with_header, length);
		decodeHeader();
	}

	// flatbuffers�� �̿��� GameMessage ������
	// data�� ����ȭ �� ���� builder��,
	// �� builder�� � data�� ��� �ִ����� ���� type ���ڸ� �־��ش�.
	GameMessage(const flatbuffers::FlatBufferBuilder& builder, int type)
	{
		setBodyLength(builder.GetSize());
		setType(type);

		std::memcpy(getBody(), builder.GetBufferPointer(), _body_length);
		encodeHeader();	
	}

	const uint8_t* getData() const
	{
		return _data;
	}

	uint8_t* getData()
	{
		return _data;
	}

	size_t getLength() const
	{
		return HEADER_LENGTH + _body_length;
	}

	const uint8_t* getBody() const
	{
		return _data + HEADER_LENGTH;
	}

	uint8_t* getBody()
	{
		return _data + HEADER_LENGTH;
	}

	size_t getBodyLength() const
	{
		return _body_length;
	}

	size_t getType() const
	{
		return _type;
	}

	void setBodyLength(size_t new_length)
	{
		_body_length = new_length;
		if (_body_length > MAX_BODY_LENGTH)
			_body_length = MAX_BODY_LENGTH;
	}

	void setType(size_t new_type)
	{
		_type = new_type;
	}

	// buffer���� �����Ͱ� ���������, �󸶸�ŭ�� ���̰� ����Ǿ����� �� ����� ����.
	// �׷��Ƿ� header�� body�� �и��ϰ�, header �κп� ���̸� ������ �ش�.
	// �� �Լ��� header�� �����͸� ���̷� ��ȯ�� �ִ� �Լ��̴�.
	// ���� �������� header�� body�� ���̰� �ٸ� ���� �̻��� �ִ� ����̰�
	// �̸� return�� ���� �˷��ش�.
	bool decodeHeader()
	{
		using namespace std; // For strncat and atoi.

		char size_header[SIZE_HEADER_LENGTH + 1];
		memcpy(size_header, _data, SIZE_HEADER_LENGTH);
		_body_length = atoi(size_header);

		char type_header[TYPE_HEADER_LENGTH + 1];
		memcpy(type_header, _data + SIZE_HEADER_LENGTH, TYPE_HEADER_LENGTH);
		_type = atoi(type_header);

		if (_body_length > MAX_BODY_LENGTH)
		{
			std::cout << "decide error" << std::endl;
			_body_length = 0;
			return false;
		}
		return true;
	}

	// �ݴ�� body�� ���̸�ŭ�� ���ۿ� �ִ� �۾�
	// size header�� type header�� ���̸�ŭ�� ������ �α� ���� setw�� ����Ѵ�.
	void encodeHeader()
	{
		std::stringstream ss;

		ss << std::setw(SIZE_HEADER_LENGTH) << _body_length;
		ss << std::setw(TYPE_HEADER_LENGTH) << _type;

		memcpy(_data, ss.str().c_str(), HEADER_LENGTH);
	}

private:
	// �������� ��ü�� �����ϴ� ����
	uint8_t _data[HEADER_LENGTH + MAX_BODY_LENGTH];

	// �������� ���̸� �����ϴ� ����
	size_t _body_length;

	// �������� Ÿ���� �����ϴ� ����
	size_t _type;
};
