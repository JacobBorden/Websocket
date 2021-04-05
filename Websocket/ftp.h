#pragma once
#ifndef FTP_H
#define FTP_H

#include "socket.h"
#include <valarray>


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
		int ReceivePasv();
		void SendCmd(Cmd cmd);
	private:
		std::string username;
		std::string address;
		int port;
		int status;
		Socket websocket;
		Socket pasv_socket;
		void Login();
		bool EnablePASV();
		bool ParsePASV();
	};
}
#endif // !FTP_H
