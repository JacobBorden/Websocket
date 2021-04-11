#include "telnet_auth.h"


AuthOctets::AuthOctets()
{
}

AuthOctets::~AuthOctets()
{
}

AuthOctets AuthOctets::ProcessAuthOctet(char octet)
{
	AuthOctets auth_octets;
	auth_octets.who_mask = octet % 2;
	auth_octets.encryption_mask = octet / 16;
	octet -= auth_octets.encryption_mask * 16;
	auth_octets.credentials_mask = octet / 8;
	octet -= auth_octets.credentials_mask * 8;
	auth_octets.encryption_mask += octet / 4;
	if (auth_octets.encryption_mask == 20 || auth_octets.encryption_mask == 4)
		octet -= 4;
	auth_octets.how_mask = octet / 2;
	return auth_octets;
}

std::string Authenticate(char authentication_type)
{
	std::string ticket;
	return ticket;
}

std::string ChallengeAuthentication()
{
	return std::string();
}

bool ValidateChallengeResponse(std::vector<char> response)
{
	return false;
}
