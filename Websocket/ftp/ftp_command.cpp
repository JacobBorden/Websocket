#include "ftp_command.h"

int ftp::Cmd::Send(Socket websocket)
{
	std::string command;
	command = code + " " + args + "\r\n";
	websocket.Send(&command[0]);
    return ReceiveResponse(websocket);
}

int ftp::Cmd::ReceiveResponse(Socket websocket)
{
	std::cout << std::endl;
	std::vector<char> data = websocket.Receive();
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << data[i];
	}
	data.resize(3);
	int status_code = atoi(&data[0]);
	return status_code;
}
