#include "socket.h"
#include "protocols.h"

int main(int argc, char* args[])
{

	if (argc != 3)
	{
		std::cout << std::endl<<"Invalid number of arguments";
		std::cout << std::endl << "Expected input: websocket.exe address port";
		return 0;
	}
	
	int port = std::atoi(args[2]);
	Socket websocket(args[1], port);
	LoadProtocol(websocket, port);
	websocket.Disconnect();
	return 0;
}