#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "tcp/socket.h"

#define PROMPT "> "

TCPSocket createConnectedTCPSocket(const char *const connectingIPAddress,
                                   const int connectingPort);
#endif // !TCP_CLIENT_H
