#include "telnet_client.h"

telnet::Client::Client(char* a, int p)
{
	host_address = a;
	port = p;
	websocket = Socket(&host_address[0], port);
	
	//Authentication
	std::vector<char> data;
	data = ReceiveData();
	
	const char do_auth[] = {IAC, DO, AUTHENTICATION };
	if (&data[0] == do_auth)
	{
		BeginAuthentication();

		if (auth_octets.encryption_mask == ENCRYPT_USING_TELOPT)
		{
			//Encrypt
		}

		if (auth_octets.credentials_mask == INI_CRED_FWD_ON)
		{
			//Login
		}

	}
	
	else ParseData(data);
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
	char will_auth[] = { IAC, WILL, AUTHENTICATION };
	websocket.Send(will_auth);
	std::vector<char> data;
	data = ReceiveData();
	char octet;
	
	if ( (data[0] == IAC) && (data[1] == SB) && (data[2] == AUTHENTICATION) && (data[3] == SEND))
	{
		octet = data[5];
		auth_octets.ProcessAuthOctet(octet);
		auth_octets.authentication_type = data[4];

		if (auth_octets.who_mask == AUTH_CLIENT_TO_SERVER)
		{
			char auth_name_header[] = { IAC, SB, AUTHENTICATION, NAME };
			char auth_name_footer[] = { IAC, SE};
			std::string auth_name = auth_name_header + GetUsername() + auth_name_footer;
			websocket.Send(&auth_name[0]);
			char auth_header[] = { IAC, SB, AUTHENTICATION, IS, auth_octets.authentication_type, octet };
			char auth_footer[] = { IAC, SE };
			std::string auth = auth_header + Authenticate(auth_octets.authentication_type, host_address) + auth_footer;
			data = ReceiveData();
			
			if (data[6] == ACCEPT && auth_octets.how_mask == AUTH_HOW_MUTUAL)
			{
				char header[] = { IAC, SB, AUTHENTICATION, IS, auth_octets.authentication_type, octet, AUTH };
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
	
	
	while(websocket.connected)
	{
	EnterCommand();
	std::vector<char>data;
	data = ReceiveData();
	ParseData(data);
	
	}
}

void telnet::Client::PrintData(std::vector<char> data)
{
	int i =0;
	char iac = IAC;
	while(i<data.size() && data[i]!=iac)
	{
		std::cout << data[i];
		i++;
	}
	
	if(data[i]==iac && data[i+1]==iac)
	{
		std::cout<<data[i];
		std::vector<char> new_data(data.begin()+i+2,data.end());
		PrintData(new_data);
	}
	else if(data[i]== iac && data[i+1]!=iac)
	{
	
		std::vector<char> new_data(data.begin()+i, data.end());
		ParseData(new_data);
	}

	
}

bool telnet::Client::NegotiateEcho(std::vector<char>data)
{
	const char will_echo[] ={IAC, WILL, ECHO};
	const char wont_echo[] ={IAC, WONT, ECHO};
	const char do_echo[]={IAC, DO, ECHO};
	const char dont_echo[] = {IAC, DONT, ECHO};

	if(&data[0] == will_echo)
	{
		Send(dont_echo);
		return true;
	}
	
	if(&data[0] == do_echo)
	{
		Send(wont_echo);
		return true;
	}

	return false;
}

bool telnet::Client::NegotiateReceiveData(std::vector<char> data)
{
const char will_send_binary_data[] = {IAC, WILL, TRANSMIT_BINARY};
const char wont_send_binary_data[] = {IAC, WONT, TRANSMIT_BINARY};

if (&data[0] == will_send_binary_data )
{
	const char do_send_data[] ={IAC, DO , TRANSMIT_BINARY};
	Send(do_send_data);
	data = ReceiveData();
	PrintData(data);
	
	return true;
}

if (&data[0] == wont_send_binary_data )
{
	const char do_send_data[] ={IAC, DONT , TRANSMIT_BINARY};
	Send(do_send_data);
	data = ReceiveData();
	PrintData(data);
	
	return true;
}

	return false;
}

void telnet::Client::ProcessCommand(std::vector<char>data)
{
	int x = data[0]; 
	int y = data[1];
	int  z = data[2];
	std::cout<<std::endl<<"Command:"<<x<<" "<<y<<" "<<z<<std::endl;
	if(!NegotiateStatus(data))
		if(!NegotiateSuppressGA(data))
			if(!NegotiateTimingMark(data))
				if(!NegotiateEcho(data))
					if(!NegotiateExtended(data))
						if(!NegotiateReceiveData(data))
							if(!NegotiateTerminalType(data))
								NegotiateScreenSize(data);
							
	
}

bool telnet::Client::NegotiateSuppressGA( std::vector<char>data)
{
	const char will_suppress_ga[] ={IAC, WILL, SUPPRESS_GO_AHEAD,};
	const char wont_suppress_ga[] ={IAC, WONT, SUPPRESS_GO_AHEAD};
	const char do_suppress_ga[] ={IAC, DO, SUPPRESS_GO_AHEAD};
	const char dont_suppress_ga[] ={IAC, DONT, SUPPRESS_GO_AHEAD};
	
	if(&data[0] == do_suppress_ga)
	{
		Send(will_suppress_ga);
		
		return true;
	}
	
if(&data[0] == will_suppress_ga)
	{
		Send(dont_suppress_ga);
	
		return true;
	}

	return false;
}

bool telnet::Client::NegotiateStatus(std::vector<char>data)
{
	const char will_status[]={IAC, WILL, STATUS };
	const char wont_status[]={IAC, WONT, STATUS};
	const char do_status[]={IAC, DO, STATUS};
	const char dont_status[]={IAC,DONT,STATUS};

	if(&data[0]== do_status)
	{
		Send(wont_status);;
		return true;
	}
	return false;
}

bool telnet::Client::NegotiateTimingMark(std::vector<char>data)
{
	const char do_timing[] ={IAC, DO, TIMING_MARK};
	const char wont_timing[] = {IAC, WONT, TIMING_MARK};
	if(&data[0]== do_timing)
	{
		Send(wont_timing);
		return true;
	}
		
	return false;
}

bool telnet::Client::NegotiateExtended(std::vector<char>data)
{
	const char do_exopl[]={IAC, DO, EXOPL};
	const char wont_exopl[]={IAC, WONT, EXOPL};
	if(&data[0]==do_exopl)
	{
		Send(wont_exopl);
		return true;
	}
	return false;
}

void telnet::Client::EnterCommand()
{
	char key;
	key=_getch();
	std::string send;

		send[0]=key;
		send[1]='\0';
		if(key=='\r')
			send = "\r\n\0";
	websocket.Send(&send[0]);
}

void telnet::Client::ParseData(std::vector<char>data)
{
		
	char iac= IAC;
		if(data[0]== iac)
		{
			int i=0;
			do{
				i++;
			}while(data[i]!=iac && i <data.size());
			std::vector<char> command(data.begin(),data.begin()+i);
			std::vector<char> new_data(data.begin()+i, data.end());
			ProcessCommand(command);
			if(i==data.size())
			{
				data = ReceiveData();
				ParseData(data);
			}
			else ParseData(new_data);
		}
			
		else PrintData(data);
}

void telnet::Client::Send(const char * data)
{
	std::string end = {'\r','\n'};
	std::string command = data + end;
	std::cout<<std::endl<<"Sent: "<<command;
	websocket.Send(&command[0]);
}

bool telnet::Client::NegotiateTerminalType(std::vector<char> data)
{
	const char do_tt[]={IAC, DO, TERMINAL_TYPE};
	const char will_tt[]={IAC,WILL,TERMINAL_TYPE};
	const char tt_send[]={IAC, SB, TERMINAL_TYPE, SEND, IAC, SE};
	
if(&data[0]== do_tt)
		{
			Send(will_tt);
			data=ReceiveData();
			if(&data[0]== tt_send)
			{	
				const char unknown_terminal[] ={IAC, SB, TERMINAL_TYPE, IS, 'U','N','K','N','O','W','N',IAC,SE};
				Send(unknown_terminal);
			}
			return true;
		}

return false;
}

bool telnet::Client::NegotiateScreenSize(std::vector<char>data)
{
	const char do_screen[]={IAC, DO, WINDOW_SIZE};
	const char will_screen[]={IAC, WILL, WINDOW_SIZE};
	if(&data[0]==do_screen)
	{
		Send(will_screen);
		HANDLE std_out;
		std_out =GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO console;
		GetConsoleScreenBufferInfo(std_out, &console);
		int width = console.dwSize.X;
		int height = console.dwSize.Y;
		int x1 = width/256;
		int x2 = width-x1 * 256;
		int y1 = height/256;
		int y2 = height-y1*256;
		const char screen_command[]={IAC, SB, WINDOW_SIZE, x1, x2, y1,y2,IAC,SE};
		Send(screen_command);
		return true;
	}
	return false;
}