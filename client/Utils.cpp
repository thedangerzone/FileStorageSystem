#include "Utils.h"

void hexify(const unsigned char* buffer, unsigned int length)
{
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}

std::string hexEncode(GUID src) {
	std::string encoded;
	CryptoPP::HexEncoder encoder;
	encoder.Put((byte*)&src, sizeof(GUID));
	encoder.MessageEnd();
	encoded.resize(static_cast<unsigned int>(encoder.MaxRetrievable()));
	encoder.Get((byte*)&encoded[0], encoded.size());
	return encoded;
}

GUID hexDecode(std::string& src) {
	GUID decoded;
	CryptoPP::HexDecoder decoder;
	decoder.Put((byte*)src.data(), src.size());
	decoder.MessageEnd();
	decoder.Get((byte*)&decoded, sizeof(GUID));
	return decoded;
}

std::string getCurrentTime() {
	time_t t = time(NULL);
	tm tlm;
	localtime_s(&tlm, &t);

	char buffer[80];
	strftime(buffer, 80, "%Y%m%dT%H%M%S", &tlm);

	return buffer;
}

