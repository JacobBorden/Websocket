#include "telnet_client.h"

telnet::Client::Client(char* a, int p)
{
	host_address = a;
	port = p;
	websocket = Socket(&host_address[0], port);

	//Authentication
	std::vector<char> data;
	data = ReceiveData();
	ParseData(data);
	data = ReceiveData();
	ParseData(data);
	MainLoop();
}

telnet::Client::~Client()
{
}

std::vector<char> telnet::Client::ReceiveData()
{
	std::vector<char> data = websocket.Receive();
	return data;
}

void telnet::Client::BeginAuthentication()
{
	char will_auth[] = { iac, will, AUTHENTICATION };
	websocket.Send(will_auth);
	std::vector<char> data;
	data = ReceiveData();
	char octet;

	if ((data[0] == iac) && (data[1] == sb) && (data[2] == AUTHENTICATION) && (data[3] == SEND))
	{
		octet = data[5];
		auth_octets.ProcessAuthOctet(octet);
		auth_octets.authentication_type = data[4];

		if (auth_octets.who_mask == AUTH_CLIENT_TO_SERVER)
		{
			char auth_name_header[] = { iac, sb, AUTHENTICATION, NAME };
			char auth_name_footer[] = { iac, se };
			std::string auth_name = auth_name_header + GetUsername() + auth_name_footer;
			websocket.Send(&auth_name[0]);
			char auth_header[] = { iac, sb, AUTHENTICATION, IS, auth_octets.authentication_type, octet };
			char auth_footer[] = { iac, se };
			std::string auth = auth_header + Authenticate(auth_octets.authentication_type, host_address) + auth_footer;
			data = ReceiveData();

			if (data[6] == ACCEPT && auth_octets.how_mask == AUTH_HOW_MUTUAL)
			{
				char header[] = { iac, sb, AUTHENTICATION, IS, auth_octets.authentication_type, octet, AUTH };
				std::string auth_challenge = header + ChallengeAuthentication() + auth_footer;
				websocket.Send(&auth_challenge[0]);
				data = ReceiveData();

				if (!ValidateChallengeResponse(data))
					websocket.Disconnect();
			}
		}
	}
}

std::string telnet::Client::GetUsername()
{
	std::string username;
	std::cout << std::endl << "Username:";
	std::cin >> username;
	return username;
}

void telnet::Client::MainLoop()
{
	std::vector<char>data;

	while (websocket.connected)
	{
		data = ReceiveData();
		ParseData(data);
		EnterCommand();
	}
}

void telnet::Client::PrintData(std::vector<char> data)
{
	int i = 0;
	while (i < data.size() && data[i] != iac)
	{
		std::cout<<data[i];
		i++;
	}
	if (i < data.size() && data[i] == iac)
	{
		if (data[i] == iac && data[i + 1] == iac)
		{
			std::cout << data[i];
			std::vector<char> new_data(data.begin() + i + 2, data.end());
			PrintData(new_data);
		}
		else if (data[i] == iac && data[i + 1] != iac)
		{
			std::vector<char> new_data(data.begin() + i, data.end());
			ParseData(new_data);
		}
	}
}

bool telnet::Client::NegotiateEcho(std::vector<char>data)
{
	std::string will_echo = { iac, will, ECHO };
	std::string wont_echo = { iac, wont, ECHO };
	std::string do_echo = { iac, _do, ECHO };
	std::string dont_echo = { iac, dont, ECHO };

	if (&data[0] == will_echo)
	{
		Send(&do_echo[0]);
		return true;
	}

	if (&data[0] == dont_echo)
	{
		Send(&wont_echo[0]);
		return true;
	}

	return false;
}

bool telnet::Client::NegotiateReceiveData(std::vector<char> data)
{
	std::string will_send_binary_data = { iac, will, TRANSMIT_BINARY };
	std::string wont_send_binary_data = { iac, wont, TRANSMIT_BINARY };

	if (&data[0] == will_send_binary_data)
	{
		std::string do_send_data = { iac, _do , TRANSMIT_BINARY };
		Send(&do_send_data[0]);
		data = ReceiveData();
		PrintData(data);

		return true;
	}

	if (&data[0] == wont_send_binary_data)
	{
		std::string do_send_data = { iac, dont , TRANSMIT_BINARY };
		Send(&do_send_data[0]);
		data = ReceiveData();
		PrintData(data);

		return true;
	}

	return false;
}

void telnet::Client::ProcessCommand(std::vector<char>data)
{
	if (!NegotiateStatus(data))
		if (!NegotiateSuppressGA(data))
			if (!NegotiateTimingMark(data))
				if (!NegotiateEcho(data))
					if (!NegotiateExtended(data))
						if (!NegotiateReceiveData(data))
							if (!NegotiateTerminalType(data))
								NegotiateScreenSize(data);
}

bool telnet::Client::NegotiateSuppressGA(std::vector<char>data)
{
	std::string will_suppress_ga = { iac, will, SUPPRESS_GO_AHEAD};
	std::string wont_suppress_ga = { iac, wont, SUPPRESS_GO_AHEAD};
	std::string do_suppress_ga = { iac, _do, SUPPRESS_GO_AHEAD};
	std::string dont_suppress_ga = { iac, dont, SUPPRESS_GO_AHEAD};
	
	if (&data[0]== do_suppress_ga)
	{
		Send(&will_suppress_ga[0]);
		return true;
	}

	if (&data[0] == will_suppress_ga)
	{
		Send(&do_suppress_ga[0]);

		return true;
	}

	return false;
}

bool telnet::Client::NegotiateStatus(std::vector<char>data)
{
	std::string will_status = { iac, will, STATUS };
	std::string wont_status = { iac, wont, STATUS };
	std::string do_status = { iac, _do, STATUS };
	std::string dont_status = { iac,dont,STATUS };

	if (&data[0] == do_status)
	{
		Send(&wont_status[0]);
		return true;
	}
	return false;
}

bool telnet::Client::NegotiateTimingMark(std::vector<char>data)
{
	std::string do_timing = { iac, _do, TIMING_MARK };
	std::string wont_timing = { iac, wont, TIMING_MARK };
	if (&data[0] == do_timing)
	{
		Send(&wont_timing[0]);
		return true;
	}

	return false;
}

bool telnet::Client::NegotiateExtended(std::vector<char>data)
{
	std::string do_exopl = { iac, _do, (char)EXOPL };
	std::string wont_exopl = { iac, wont, (char)EXOPL };
	if (&data[0] == do_exopl)
	{
		Send(&wont_exopl[0]);
		return true;
	}
	return false;
}

void telnet::Client::EnterCommand()
{
	char key;
	key = _getch();
	std::string send;
	send = key;
	if (key == '\r')
		send = "\r\n";
	websocket.Send(&send[0]);
}

void telnet::Client::ParseData(std::vector<char>data)
{
	if (data[0] == iac)
	{
		int i = 0;
		do {
			i++;
		} while (data[i] != iac && i < data.size()-1);
		std::vector<char> command(data.begin(), data.begin() + i);
		std::vector<char> new_data(data.begin() + i, data.end());
		ProcessCommand(command);
		if (i == data.size())
		{
			data = ReceiveData();
			ParseData(data);
		}
		else ParseData(new_data);
	}

	else PrintData(data);
}

void telnet::Client::Send(const char* data)
{
	std::string end = { '\r','\n' };
	std::string command = data + end;
	websocket.Send(&command[0]);
}

bool telnet::Client::NegotiateTerminalType(std::vector<char> data)
{
	std::string do_tt = { iac, _do, TERMINAL_TYPE };
	std::string will_tt = { iac,will,TERMINAL_TYPE };
	std::string tt_send = { iac, sb, TERMINAL_TYPE, SEND, iac, se };

	if (&data[0] == do_tt)
	{
		Send(&will_tt[0]);
		data = ReceiveData();
		std::string unknown_terminal = { iac, sb, TERMINAL_TYPE, IS, 'U','N','K','N','O','W','N',iac,se };
		Send(&unknown_terminal[0]);
		
		return true;
	}

	return false;
}

bool telnet::Client::NegotiateScreenSize(std::vector<char>data)
{
	std::string do_screen = { iac, _do, WINDOW_SIZE };
	std::string will_screen = { iac, will, WINDOW_SIZE };
	if (&data[0] == do_screen)
	{
		Send(&will_screen[0]);
		HANDLE std_out;
		std_out = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO console;
		GetConsoleScreenBufferInfo(std_out, &console);
		int width = console.dwSize.X;
		int height = console.dwSize.Y;
		int x1 = width / 256;
		int x2 = width - x1 * 256;
		int y1 = height / 256;
		int y2 = height - y1 * 256;
		std::string screen_command = { iac, sb, WINDOW_SIZE, (char)x1, (char)x2, (char)y1, (char)y2,iac,se };
		Send(&screen_command[0]);
		return true;
	}
	return false;
}