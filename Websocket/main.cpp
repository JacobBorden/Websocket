#include "socket.h"
#include "protocols.h"

int main(int argc, char* args[])
{

	if (argc != 2)
	{
		std::cout << std::endl<<"Invalid number of arguments";
		std::cout << std::endl << "Expected input: websocket.exe address port";
		return 0;
	}
	
	int port = std::atoi(args[1]);
	Socket websocket(args[0], port);
	LoadProtocol(websocket, port);
	return 0;
}