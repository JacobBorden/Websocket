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

	int ftp::Client::ReceivePasv()
	{
		std::cout << std::endl;
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
				if (!port_enabled)
					EnablePASV();
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

			else if (command == "cd" || command == "cwd" || command=="CWD")
			{
				Cmd cwd;
				cwd.code = "CWD";
				cwd.args = args_vector[0];
				SendCmd(cwd);
				status = ReceiveData();
			}

			else if (command == "mkdir" || command == "mkd" || command == "MKD")
			{
				Cmd mkd;
				mkd.code = "MKD";
				mkd.args = args_vector[0];
				SendCmd(mkd);
				status = ReceiveData();
			}

			else if (command == "rmdir" || command == "rmd" || command == "RMD")
			{
				Cmd rmd;
				rmd.code = "RMD";
				rmd.args = args_vector[0];
				SendCmd(rmd);
				status = ReceiveData();
			}

			else if (command == "rm" || command == "del" || command == "delete" || command == "dele" || command == "DELE")
			{
				Cmd dele;
				dele.code = "DELE";
				dele.args = args_vector[0];
				SendCmd(dele);
				status = ReceiveData();
			}

			else if (command == "rename" || command == "mv" || command =="ren" )
			{
				Cmd rnfr;
				rnfr.code = "RNFR";
				rnfr.args = args_vector[0];
				SendCmd(rnfr);
				status = ReceiveData();
				Cmd reto;
				reto.code = "RETO";
				reto.args = args_vector[1];
				SendCmd(reto);
				status = ReceiveData();
			}
				
			else if (command == "get" || command == "retr" || command == "RETR")
			{

				if (!port_enabled)
					EnablePASV();
				Cmd retr;
				retr.code = "RETR";
				retr.args = args_vector[0];
				SendCmd(retr);
				status = ReceiveData();
				if (status != 550)
				{
					GetFile(args_vector[0]);
					status = ReceiveData();
				}
			}

			else if (command == "put" || command == "send" || command == "STOR" || command == "stor")
			{

				if (!port_enabled)
					EnablePASV();
				Cmd stor;
				stor.code = "STOR";
				stor.args = args_vector[0];
				SendCmd(stor);
				status = ReceiveData();
				if (status == 150)
				{
					
					SendFile(args_vector[0]);
					status = ReceiveData();
				}
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

	void ftp::Client::GetFile(std::string filename)
	{
		std::vector<char>data = pasv_socket.Receive();
		DWORD bytes_written;
		HANDLE file;
		file = CreateFileA(&filename[0], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(file, &data[0], data.size(), &bytes_written, NULL);
		CloseHandle(file);

	}

	void ftp::Client::SendFile(std::string filename)
	{
		std::vector<BYTE> buffer;
		DWORD bytes_written;
		HANDLE file;
		bool success;
		file = CreateFileA(&filename[0], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		do
		{
			int i = buffer.size();
			buffer.resize(buffer.size() + 512);
			success = ReadFile(file, &buffer[i], 512, &bytes_written, NULL);
		} while (bytes_written > 0 && success);
		CloseHandle(file);

		pasv_socket.Send((char*)&buffer[0]);
		pasv_socket.Disconnect();
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
			int i = 0;
			char key = NULL;
			char backspace = '\b';
			char enter = '\r';
			std::vector <char> password;
			while (key != enter)
			{
				key = _getch();
				if (key != enter)
				{
					if (key == backspace && password.size() > 0)
					{
						password.resize(password.size() - 1);
						i--;
						std::cout << "\b \b";

					}
					else if (key != backspace)
					{
						i++;
						password.resize(i);
						password[i - 1] = key;
						std::cout << '*';
					}

				}
			}
			i++;
			password.resize(i);
			password[i - 1] = '\0';

			pass.args = &password[0];
			//std::cin >> pass.args;
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