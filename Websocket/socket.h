#pragma once
#ifndef SOCKET_H
#define SOCKET_H
#include <WinSock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <vector>

class Socket
{
public:
	
	Socket(char* web_address, int port);
	bool Send(char* send_buffer);
	std::vector<char>  Receive();
	~Socket();

private:
	
	bool InitSocket();
	bool CreateSocket(char* web_address, int port);
	bool ConnectSocket();
	
	WSADATA wsadata;
	addrinfo* response = NULL;
	SOCKET connection_socket = INVALID_SOCKET;
	
};

#endif // !SOCKET_H

