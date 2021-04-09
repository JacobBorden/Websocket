#pragma once
#ifdef FTP_DLL_EXPORTS
#define FTP_DLL_API __declspec(dllexport)
#else
#define FTP_DLL_API __declspec(dllimport)
#endif // FTP_DLL_EXPORTS

#include "../ftp_client.h"


namespace ftp
{
	extern "C++" FTP_DLL_API void LoadFTP(char* address, int port);
}
