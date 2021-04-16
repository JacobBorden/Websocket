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
	const char auth[] = {IAC, WILL, AUTHENTICATION};
	Send(auth);
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
	
	std::vector<char>data;
	data = ReceiveData();
	ParseData(data);	
	EnterCommand();
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

	if(!NegotiateStatus(data))
		if(!NegotiateSuppressGA(data))
			if(!NegotiateTimingMark(data))
				if(!NegotiateEcho(data))
					if(!NegotiateExtended(data))
						NegotiateReceiveData(data);
							
	
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
		Send(do_suppress_ga);
	
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
				
			}
			else ParseData(new_data);
		}
			
		else PrintData(data);
}

void telnet::Client::Send(const char * data)
{
	std::string end = {'\r','\n'};
	std::string command = data + end;
	websocket.Send(&command[0]);
}