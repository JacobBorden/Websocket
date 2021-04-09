#pragma once
#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H 


#include "../socket/socket.h"
#include "ftp_command.h"
#include <valarray>
#include <sstream>
#include <conio.h>




namespace ftp
{


	class Client
	{
	public:
		Client(char* address, int port);
		Client();
		~Client();
		
	private:
		std::string username;
		std::string address;
		int port;
		int status;
		Socket websocket;
		Socket pasv_socket;
		bool port_enabled = false;
		void Login();
		bool EnablePASV();
		bool ParsePASV();
		void InputLoop();
		void GetFile(std::string filename);
		void SendFile(std::string filename);
		
	};
}

#endif