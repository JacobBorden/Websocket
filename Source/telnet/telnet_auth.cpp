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

std::string Authenticate(char authentication_type, std::string host_address)
{
	std::string ticket;
	if (authentication_type == KERBEROS_V5)
		ticket = LoadKerberos5KeyTab(host_address);
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

std::string LoadKerberos5KeyTab(std::string host_address)
{
	std::string ticket;
	std::string filename;
	GetPrivateProfileStringA("Kerberos 5", &host_address[0], NULL, &filename[0],sizeof(filename),"telnet.cfg");
	DWORD bytes_read;
	HANDLE file;
	file = CreateFileA(&filename[0], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(file, &ticket[0], bytes_read, &bytes_read, NULL);
	CloseHandle(file);
	return ticket;
}
