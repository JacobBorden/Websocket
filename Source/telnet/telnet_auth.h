#pragma once
#ifndef TELNET_AUTH_H
#define TELNET_AUTH_H

#include <iostream>
#include <vector>

#define AUTHENTICATION 37
//Sub-Commands
#define IS 0
#define SEND 1
#define REPLY 2
#define NAME 3
//Sub-Options
#define AUTH 0
#define REJECT 1
#define ACCEPT 2
#define CHALLENGE 3
#define RESPONSE 4
//Authentication Types
#define KERBEROS_V4 1
#define KERBEROS_V5 2
#define SPX 3
#define MINK 4
#define SRP 5
#define RSA 6
#define LOKI 10
#define SSA 11
#define KEA_SJ 12
#define KEA_SJ_INTEG 13
#define DSS 14
#define NTLM 15

//Modifiers
//Who Mask
#define AUTH_CLIENT_TO_SERVER 0
#define AUTH_SERVER_TO_CLIENT 1
//How Mask
#define AUTH_HOW_ONE_WAY 0
#define AUTH_HOW_MUTUAL 1
//Encryption Mask
#define ENCRYPT_OFF 0
#define ENCRYPT_USING_TELOPT 4
#define ENCRYPT_AFTER_EXCHANGE 16
#define ENCRYPT_RESERVED 20
//Credentials Mask
#define INI_CRED_FWD_OFF 0
#define INI_CRED_FWD_ON 1

class AuthOctets
{
public:
	AuthOctets();
	~AuthOctets();
	char authentication_type =0;
	bool who_mask = 0;
	bool how_mask = 0;
	bool credentials_mask = 0;
	char encryption_mask =0;
	AuthOctets ProcessAuthOctet(char octet);

};

std::string Authenticate(char authentication_type);
std::string ChallengeAuthentication();
bool ValidateChallengeResponse(std::vector<char> response);
#endif // !TELNET_AUTH_H

