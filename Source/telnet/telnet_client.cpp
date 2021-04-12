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
	}
	if (auth_octets.encryption_mask == ENCRYPT_USING_TELOPT)
	{
		//Encrypt
	}
	if (auth_octets.credentials_mask == INI_CRED_FWD_ON)
	{
		//Login
	}
}

telnet::Client::~Client()
{
}

std::vector<char> telnet::Client::ReceiveData()
{
	std::vector<char> data = websocket.Receive();
	std::cout << std::endl;
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << data[i];
	}
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

