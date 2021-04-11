#pragma once
#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include "../../ftp/ftp.h"
#include "../../telnet/telnet.h"

void LoadProtocol(char* address, int port);
void LoadProtocol(char* address, int port, std::string protocol);

#endif // !PROTOCOLS_H
