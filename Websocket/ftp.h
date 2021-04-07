#pragma once
#ifndef FTP_H
#define FTP_H

#include "socket.h"
#include <valarray>
#include <sstream>


void LoadFTP(char* address, int port);

namespace ftp
{
	struct Cmd {
		std::string code = "\0";
		std::string args = "\0";
	};

	class Client
	{
	public:
		Client(char* address, int port);
		~Client();
		
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
		int ReceiveData();
		int ReceivePasv();
		void SendCmd(Cmd cmd);
		void InputLoop();
		void GetFile(std::string filename);
		
	};
}
#endif // !FTP_H
