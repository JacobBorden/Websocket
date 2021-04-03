#include "socket.h"

Socket::Socket(char* web_address, int port)
{
	bool startup = InitSocket();
	if (startup)
	{
		bool create_sock = CreateSocket(web_address, port);
		if (create_sock)
		{
			bool connect = ConnectSocket();
			if (connect)
			{
				std::cout << std::endl << "Successfully Connected to " << web_address << " on port " << port;
			}
		}
	}
		
}

bool Socket::InitSocket()
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0)
	{
		std::cout << std::endl << "WSAStartup failed: "<<result;
		return false;
	}

	return true;
}

bool Socket::CreateSocket(char* web_address, int port)
{
	int length = log10(port) +2;
	char* port_info = new char[length];
	_itoa_s(port, port_info, length, 10);
	port_info[length] = '\n';
	addrinfo address_info;
	ZeroMemory(&address_info, sizeof(address_info));
	address_info.ai_family = AF_UNSPEC;
	address_info.ai_socktype = SOCK_STREAM;
	address_info.ai_protocol = IPPROTO_TCP;
	int result = getaddrinfo((PCSTR)web_address, (PCSTR)port_info, &address_info, &response);
	if (result != 0)
	{
		std::cout << std::endl << "getaddrinfo failed: " << result;
		WSACleanup();
		return false;
	}

	connection_socket = socket(response->ai_family, response->ai_socktype, response->ai_protocol);
	if (connection_socket == INVALID_SOCKET)
	{
		std::cout << std::endl << "Socket Error: " << WSAGetLastError();
		freeaddrinfo(response);
		WSACleanup();
		return false;
	}
	return true;
}

bool Socket::ConnectSocket()
{
	int result = connect(connection_socket, response->ai_addr, response->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		closesocket(connection_socket);
		connection_socket = INVALID_SOCKET;
		std::cout << std::endl << "Could not connect";
		WSACleanup();
		return false;
	}

	freeaddrinfo(response);
	return true;
}

bool Socket::Send(char* send_buffer)
{
	int result = send(connection_socket, send_buffer, strlen(send_buffer), 0);
	if (result == SOCKET_ERROR)
	{
		std::cout << std::endl << "Send failed: " << WSAGetLastError();
		closesocket(connection_socket);
		WSACleanup();
		return false;
	}

	std::cout << std::endl << "Succesfully sent " << result << " bytes";

	result = shutdown(connection_socket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		std::cout << std::endl << "Shutdown failed: " << WSAGetLastError();
		closesocket(connection_socket);
		WSACleanup();
		return false;
	}

	return true;
}

std::vector<char> Socket::Receive()
{
	int result;
	std::vector<char> buffer;
	do 
	{
		int i = buffer.size();
		buffer.resize(buffer.size() + 512);
		result = recv(connection_socket, &buffer[i], 512, 0);

		if (result < 0)
		{
			std::cout << std::endl << "Recieve failed: " << WSAGetLastError();
			closesocket(connection_socket);
			WSACleanup();
		}
		else if (result == 0)
		{
			std::cout << std::endl << "Connection closed.";
			closesocket(connection_socket);
			WSACleanup();
		}
		else if (result > 0)
		{
			std::cout << std::endl << "Succesfully recieved " << result << " bytes";
		}
	} 
	while (result >0);

	return buffer;
}

Socket::~Socket()
{
}
