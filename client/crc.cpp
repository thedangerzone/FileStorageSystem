#include "crc.h"


uint32_t toUnsigned(uint32_t byte) { return byte & 0xffffffff; }

uint32_t calcCRC(std::vector<unsigned char> fileContent) {
	int length = fileContent.size();
	uint32_t crc = 0;
	int tableIndx;
	unsigned char currentByteLength;

	for (unsigned char byte : fileContent) {
		tableIndx = (crc >> 24) ^ byte;
		crc = toUnsigned((crc << 8)) ^ crctab[tableIndx];
	}
	while(length != 0) {
		currentByteLength = length & 255;
		length = length >> 8;
		crc = toUnsigned((crc << 8)) ^ crctab[(crc >> 24) ^ currentByteLength];
	}

	return ~toUnsigned(crc);
}