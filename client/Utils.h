#pragma once
#include <hex.h>
#include <iostream>
#include <guiddef.h>
#include <iomanip>
#include <winsock2.h>
#include <ctime>

using CryptoPP::HexEncoder;

std::string hexEncode(GUID src);
GUID hexDecode(std::string& src);
std::string getCurrentTime();
