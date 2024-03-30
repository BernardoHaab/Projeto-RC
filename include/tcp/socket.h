#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "netinet/in.h"

#include <sys/types.h>

#define BUFFER_SIZE 1024

typedef struct {
	int fileDescriptor;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE + 1];
} TCPSocket;

struct sockaddr_in createSocketAddress(const char *const ipAddress,
                                       const int port);
TCPSocket createEmptyTCPSocket(void);

ssize_t readFromTCPSocket(TCPSocket *tcpSocket);
void closeTCPSocket(TCPSocket *tcpSocket);
void writeToTCPSocketBuffer(TCPSocket *tcpSocket, const char *const string);
void writeToTCPSocket(TCPSocket *tcpSocket);

#endif // !TCP_SOCKET_H
