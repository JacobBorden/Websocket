#pragma once
#ifdef TELNET_DLL_EXPORTS
#define TELNET_DLL_API __declspec(dllexport)
#else
#define TELNET_DLL_API __declspec(dllimport)
#endif // TELNET_DLL_EXPORTS

#include "../Source/telnet/telnet_client.h"

namespace telnet
{
	extern "C++" TELNET_DLL_API void LoadTelnet(char* address, int port);
}