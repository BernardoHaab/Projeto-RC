#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <netinet/in.h>

#define BUFFER_SIZE 1024

typedef struct {
	int fileDescriptor;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE + 1];
} UDPSocket;

UDPSocket createUDPSocket(const char *const ipAddress, const int port);
UDPSocket receiveFromUDPSocket(UDPSocket *udpSocket);
void closeUDPSocket(UDPSocket *udpSocket);
void sendToUDPSocket(UDPSocket *udpSocket);
void writeToUDPSocketBuffer(UDPSocket *udpSocket, const char *const string);

#endif // !UDP_SOCKET_H
