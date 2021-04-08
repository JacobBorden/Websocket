#pragma once
#ifndef FTP_COMMAND_H
#define FTP_COMMAND_H
#include <iostream>
#include "../socket/socket.h"
#include "ftp_command.h"

namespace ftp {

	class Cmd
	{
	
	public:
		std::string code = "\0";
		std::string args = "\0";
		int Send(Socket websocket);
		static int ReceiveResponse(Socket websocket);
	};
	
}


#endif // !FTP_COMMAND_H
