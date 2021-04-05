#pragma once
#ifndef FTP_H
#define FTP_H

#include "socket.h"



void LoadFTP(char* address, int port);

namespace ftp
{
	struct Cmd {
		std::string code = "\n";
		std::string args = "\n";
	};

	class Client
	{
	public:
		Client(char* address, int port);
		~Client();
		int ReceiveData();
		void SendCmd(Cmd cmd);
	private:
		std::string username;
		std::string address;
		int port;
		int status;
		Socket websocket;
		void Login();
		
	};
}
#endif // !FTP_H
