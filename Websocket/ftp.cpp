#include "ftp.h"


	void LoadFTP(char* address, int port)
	{
		
		ftp::Client ftp(address, port);
		
		
	}

		
	ftp::Client::Client(char* a, int p)
	{
		address = a;
		port = p;
		websocket = Socket(&address[0], port);
		if (websocket.connected)
		{
			//DeclareUTF8();
			Login();
		}
		websocket.Disconnect();
	}

	ftp::Client::~Client()
	{
	}

	int ftp::Client::ReceiveData()
	{
		std::vector<char> data = websocket.Receive();
		for (int i = 0; i < data.size(); i++)
		{
			std::cout << data[i];
		}
		data.resize(3);
		int status_code = atoi(&data[0]);
		return status_code;
	}

	void ftp::Client::SendCmd(Cmd cmd)
	{
		std::string command;
		
		command = cmd.code + " " + cmd.args + "\n";
		websocket.Send(&command[0]);
	}

	void ftp::Client::Login()
	{
		
		status = ReceiveData();
		if (status == 220)
		{
			Cmd user;
			user.code = "USER";
			std::cout << std::endl << "Username (" << address << "):";
			std::cin >> user.args;
			username = user.args;
			SendCmd(user);
			status = ReceiveData();
		}
		
		if (status == 331)
		{
			Cmd pass;
			pass.code = "PASS";
			std::cout << std::endl << "Password (" << username << "@" << address << "):";
			std::cin >> pass.args;
			SendCmd(pass);
			status = ReceiveData();
		}
		
		if (status == 230)
		{
			
			

			std::string command;
			while (status != 221)
			{
				std::cout << std::endl << username << "@" << address<<">";
				
				std::cin >> command;
				command = command + "\n";
				websocket.Send(&command[0]);
				status = ReceiveData();
				
			}
			
		}

	}

	
	


