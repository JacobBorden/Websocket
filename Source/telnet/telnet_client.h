#pragma once
#ifndef TELNET_CLIENT_H
#define TELNET_CLIENT_H

#include "../socket/socket.h"
#include "telnet_auth.h"
#include <iostream>

//Telnet Command Codes
#define IAC 0xFF
#define WILL 0xFB
#define WONT 0xFC
#define DO 0xFD
#define DONT 0xFE

#define SB 250 //Subnegotiation
#define SE 240 //End Subnegotiation

namespace telnet
{
	class Client
	{
	public:
		Client(char* address, int port);
		~Client();
	
	private:
		Socket websocket;
		AuthOctets auth_octets;
		std::string host_address;
		int port;
		std::vector<char> ReceiveData();
		void BeginAuthentication();
		std::string GetUsername();
	};
}


#endif // !TELNET_CLIENT_H
