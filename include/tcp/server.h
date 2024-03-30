#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcp/socket.h"

TCPSocket createListeningTCPSocket(const char *const listeningIPAddress,
                                   const int listeningPort);

#endif // !TCP_SERVER_H
