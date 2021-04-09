#include "pch.h"
#include "ftp.h"

 

void ftp::LoadFTP(char* address, int port)
{
	ftp::Client ftp_client(address, port);
}
