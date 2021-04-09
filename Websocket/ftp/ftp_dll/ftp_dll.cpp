#include "pch.h"
#include "ftp_dll.h"

 

void ftp::LoadFTP(char* address, int port)
{
	ftp::Client ftp_client(address, port);
}
