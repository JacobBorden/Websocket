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
			
			
			Login();
			if (status == 230)
			{
				InputLoop();
			}
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

	int ftp::Client::ReceivePasv()
	{
		std::vector<char> data = pasv_socket.Receive();
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

	void ftp::Client::InputLoop()
	{
		std::string command;
		std::string args;
		std::string parser;
		std::vector<std::string> args_vector;
		while (status != 221)
		{
			std::cout << std::endl << username << "@" << address << ">";

			std::cin >> command;
			std::getline(std::cin, args);
			std::istringstream iss(args);
			int i = 0;
			while(iss>>parser)
			{

				args_vector.resize(args_vector.size() + 1);
				args_vector[i] = parser;
				i++;
			}


			if (command == "PASV" || command =="pasv")
			{
				EnablePASV();

			}

						
			else if (command == "ls" || command == "list" || command == "dir" || command == "LIST")
			{
				Cmd list;
				list.code = "LIST";
				SendCmd(list);
				status = ReceiveData();
				ReceivePasv();
				status = ReceiveData();
			}

			else if (command == "cd.." || (command == "cd" && args_vector[0] == "..") || command == "cdup" || command == "CDUP")
			{
				Cmd cdup;
				cdup.code = "CDUP";
				SendCmd(cdup);
				status = ReceiveData();
			}

			else if (command == "cd")
			{
				Cmd cd;
				cd.code = "CWD";
				cd.args = args_vector[0];
				SendCmd(cd);
				status = ReceiveData();
			}

			else
			{	
				Cmd cmd;
				cmd.code = command;
				cmd.args = args_vector[0];
				SendCmd(cmd);
				status = ReceiveData();
			}


		}
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
				
	}

	bool ftp::Client::EnablePASV()
	{
		Cmd pasv;
		pasv.code = "PASV";
		SendCmd(pasv);
		return ParsePASV();
		
		
	}

	bool ftp::Client::ParsePASV()
	{
		std::vector<char> data = websocket.Receive();
		std::cout << std::endl << &data[0];
		std::valarray<char> data_array(&data[0], data.size());
		std::valarray<char> header_code = data_array[std::slice(0, 3, 1)];
		status = atoi(&header_code[0]);
		if (status != 227)
			return false;
		std::valarray<char> ip= data_array[std::slice(27, data_array.size() - 28, 1)];
		int i = 0, comma = 0;
		std::vector<char> pasv_address(0);
		std::vector<char> port_string(0);
		while (comma < 4)
		{
			if (ip[i] != ',')
			{
				pasv_address.resize(pasv_address.size()+1);
				pasv_address[i] = ip[i];
				i++;
			}
			else
			{
				comma++;
				if (comma < 4)
				{
					pasv_address.resize(pasv_address.size() + 1);
					pasv_address[i] = '.';
					i++;
				}
				
			}
			
		}
		i++;
		int j = 0;
		while (ip[i] != ',')
		{
			port_string.resize(port_string.size() + 1);
			port_string[j] = ip[i];
			j++;
			i++;
		}
		int pasv_port = atoi(&port_string[0]) * 256;
		i++;
		j = 0;
		port_string.resize(0);
		while (ip[i] != ')')
		{
			port_string.resize(port_string.size() + 1);
			port_string[j] = ip[i];
			j++;
			i++;
		}
		pasv_port += atoi(&port_string[0]);
		pasv_address.resize(pasv_address.size() + 1);
		pasv_address[pasv_address.size() - 1] = '\0';
		pasv_socket = Socket(&pasv_address[0], pasv_port);
		bool pasv_successful = pasv_socket.connected;
		return pasv_successful;
	}