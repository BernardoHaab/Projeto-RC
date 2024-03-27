#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <netinet/in.h>

#define BUFFER_SIZE       1024
#define SERVER_IP_DEFAULT "0.0.0.0"

typedef struct {
	int fileDescriptor;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE + 1];
} UDPSocket;

UDPSocket createUDPSocket(const char *const ipAddress, const int port);
struct sockaddr_in receiveFromUDPSocket(UDPSocket *udpSocket);

#endif // !UDP_SERVER_H
