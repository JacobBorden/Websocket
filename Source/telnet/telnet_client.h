#pragma once
#ifndef TELNET_CLIENT_H
#define TELNET_CLIENT_H

#include "../socket/socket.h"
#include "telnet_auth.h"
#include <iostream>
#include "telnet_command_codes.h"
#include "telnet_option_codes.h"

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
		void PrintData(std::vector<char> data);
		void BeginAuthentication();
		void MainLoop();
		bool NegotiateReceiveData(std::vector<char>);
		bool NegotiateEcho(std::vector<char>data);
		bool NegotiateSuppressGA(std::vector<char>data);
		void ProcessCommand(std::vector<char>data);
		std::string GetUsername();
	};
}


#endif // !TELNET_CLIENT_H
