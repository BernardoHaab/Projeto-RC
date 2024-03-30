#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "udp/socket.h"

UDPSocket createBindedUDPSocket(const char *const bindingIPAddress,
                                const int bindingPort);

#endif // !UDP_SERVER_H
