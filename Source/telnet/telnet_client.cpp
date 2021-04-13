#include "telnet_client.h"

telnet::Client::Client(char* a, int p)
{
	host_address = a;
	port = p;
	websocket = Socket(&host_address[0], port);
	
	//Authentication
	std::vector<char> data;
	data = ReceiveData();
	const char do_auth[] = { IAC, DO, AUTHENTICATION };
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
	std::vector<char> data;
	data = ReceiveData();
	if(!NegotiateEcho(data))
		if(!NegotiateSuppressGA(data))
			NegotiateReceiveData(data);

}

void telnet::Client::PrintData(std::vector<char> data)
{
	for(int i=0; i< data.size(); i++)
	{
		std::cout << data[i];
	}
}

bool telnet::Client::NegotiateEcho(std::vector<char>data)
{
	const char will_echo[] ={IAC, WILL, ECHO};
	const char wont_echo[] ={IAC, WONT, ECHO};
	const char do_echo[]={IAC, DO, ECHO};
	const char dont_echo[] = {IAC, DONT, ECHO};

	if(&data[0] == will_echo || &data[0] == wont_echo)
	{
		websocket.Send((char*)wont_echo);
		websocket.Send((char*)dont_echo);
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
	data = ReceiveData();
	PrintData(data);
	
	return true;
}

if (&data[0] == wont_send_binary_data )
{
	const char do_send_data[] ={IAC, DONT , TRANSMIT_BINARY};
	data = ReceiveData();
	PrintData(data);
	
	return true;
}

	return false;
}

void telnet::Client::ProcessCommand(std::vector<char>data)
{
	
	
}

bool telnet::Client::NegotiateSuppressGA( std::vector<char>data)
{
	const char will_suppress_ga[] ={IAC, WILL, SUPPRESS_GO_AHEAD};
	const char wont_suppress_ga[] ={IAC, WONT, SUPPRESS_GO_AHEAD};
	const char do_suppress_ga[] ={IAC, DO, SUPPRESS_GO_AHEAD};
	const char dont_suppress_ga[] ={IAC, DONT, SUPPRESS_GO_AHEAD};
	
	if(&data[0] == will_suppress_ga || &data[0] == wont_suppress_ga)
	{
		websocket.Send((char*)wont_suppress_ga);
		websocket.Send((char*)dont_suppress_ga);
		return true;
	}
	
	return false;
}