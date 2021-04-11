#include "pch.h"
#include "telnet.h"

void telnet::LoadTelnet(char* address, int port)
{
	telnet::Client telnet_client(address, port);
}