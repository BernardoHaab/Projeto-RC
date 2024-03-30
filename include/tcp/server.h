#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcp/socket.h"

TCPSocket createListeningTCPSocket(const char *const listeningIPAddress,
                                   const int listeningPort);
TCPSocket acceptFromTCPSocket(const TCPSocket listeningTCPSocket);

#endif // !TCP_SERVER_H
