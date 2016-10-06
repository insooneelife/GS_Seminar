
#include <iostream>
#include <bitset>
#include <algorithm>
#include "MemoryBitStream.h"
using namespace std;

namespace _ex1
{
	char		_buffer[16];
	uint32_t	_bit_head;

	template<typename T>
	void printBinary(T val)
	{
		constexpr int size = 8 * sizeof(T);
		bitset<size> bit(val);
		for (int i = size - 1; i >= 0; --i)
			cout << bit[i];
		cout << endl;
	}

	void printBufferState(int printSize)
	{
		cout << "bit head : " << _bit_head << endl;
		cout << "bit buffer" << endl;

		for (int i = 0; i < printSize; i++)
			printBinary(_buffer[i]);
		cout << endl;
	}

	void writeBits(uint8_t data, uint32_t bit_cnt)
	{
		uint32_t nextBitHead = _bit_head + static_cast<uint32_t>(bit_cnt);

		// Calculate the byteOffset into our buffer
		// by dividing the head by 8
		// and the bitOffset by taking the last 3 bits
		uint32_t byteOffset = _bit_head >> 3;
		uint32_t bitOffset = _bit_head & 0x7;

		uint8_t currentMask = ~(0xff << bitOffset);
		_buffer[byteOffset] = (_buffer[byteOffset] & currentMask) | (data << bitOffset);

		//calculate how many bits were not yet used in
		//our target byte in the buffer
		uint32_t bitsFreeThisByte = 8 - bitOffset;

		//if we needed more than that, carry to the next byte
		if (bitsFreeThisByte < bit_cnt)
		{
			//we need another byte
			_buffer[byteOffset + 1] = data >> bitsFreeThisByte;
		}

		_bit_head = nextBitHead;
	}

	void writeBits(const void* data, uint32_t bit_cnt)
	{
		const char* src_byte = static_cast<const char*>(data);

		//write all the bytes
		while (bit_cnt > 8)
		{
			writeBits(*src_byte, 8);
			++src_byte;
			bit_cnt -= 8;
		}
		//write anything left
		if (bit_cnt > 0)
		{
			writeBits(*src_byte, bit_cnt);
		}
	}

	

	void example()
	{
		int a = 14;
		writeBits(&a, sizeof(a) * 8);
		printBufferState(4);

		float b = 14.134f;
		writeBits(&b, sizeof(b) * 8);
		printBufferState(8);

		double c = 5661.994;
		writeBits(&c, sizeof(c) * 8);
		printBufferState(16);

	}
}

namespace _ex2
{
	template<typename T>
	void printBinary(T val)
	{
		constexpr int size = 8 * sizeof(T);
		bitset<size> bit(val);
		for (int i = size - 1; i >= 0; --i)
			cout << bit[i];
		cout << endl;
	}

	void printBufferToBinary(char* buffer, int length)
	{
		for (int i = 0; i < length; i++)
		{
			printBinary(buffer[i]);
		}
	}

	void example()
	{
		OutputMemoryBitStream out;
		int a = 8;
		float b = 17.75f;
		std::string c = "abcd";

		out.write(a);
		out.write(b);
		out.write(c);

		InputMemoryBitStream inputStream(const_cast<char*>(out.getBuffer()), out.getBitLength());

		std::cout << out.getByteLength() << " " << out.getBitLength() << endl;;
		printBufferToBinary(const_cast<char*>(out.getBuffer()), out.getByteLength());

		int ia;
		inputStream.read(ia);
		std::cout << ia << std::endl;

		float ib;
		inputStream.read(ib);
		std::cout << ib << std::endl;

		std::string ic;
		inputStream.read(ic);
		std::cout << ic << std::endl;
	}
}



int main()
{
	_ex1::example();
	

	return 0;
}

