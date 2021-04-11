#include "./protocols/protocols.h"
#include "command.h"
#include "help.h"

int main(int argc, char* args[])
{
	if (argc == 1)
	{
		LoadCommandPrompt();
	}
	
	else if (argc == 3)
	{
		int port = std::atoi(args[2]);
		LoadProtocol(args[1], port);
	}
	
	
	else if (argc == 4)
	{
		int port = std::atoi(args[2]);
		LoadProtocol(args[1], port, args[3]);
	}

	else
	{
		std::cout << std::endl << "Unknown argument.";
	}
		
	return 0;
}