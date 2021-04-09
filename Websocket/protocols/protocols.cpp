#include "protocols.h"
#include "../ftp/ftp_dll/ftp_dll.h"
#include "../ftp/ftp_client.h"

void LoadProtocol(char* address, int port)
{
	switch (port)
	{
	
	case 21:
		ftp::LoadFTP(address, port);
		break;
	case 22:
		break;
	case 23:
		break;
	case 25:
		break;
	case 53:
		break;
	case 67:
		break;
	case 68:
		break;
	case 80:
		break;
	case 110:
		break;
	case 119:
		break;
	case 123:
		break;
	case 143:
		break;
	case 161:
		break;
	case 194:
		break;
	case 443:
		break;
	}
}

