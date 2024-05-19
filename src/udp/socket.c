#include "udp/socket.h"

#include "debug.h"

#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

UDPSocket receiveFromUDPSocket(UDPSocket *udpSocket)
{
	UDPSocket clientUDPSocket = {0};

	clientUDPSocket.fileDescriptor  = udpSocket->fileDescriptor;
	socklen_t clientIPAddressLength = sizeof(clientUDPSocket.address);

	int receivedBytes = -1;
	if ((receivedBytes
	     = recvfrom(udpSocket->fileDescriptor,
	                udpSocket->buffer,
	                BUFFER_SIZE,
	                0,
	                (struct sockaddr *) &clientUDPSocket.address,
	                &clientIPAddressLength))
	    < 0) {
		error("recvfrom: ");
		return clientUDPSocket;
	}

	udpSocket->buffer[receivedBytes] = '\0';

	return clientUDPSocket;
}

void closeUDPSocket(UDPSocket *udpSocket)
{
	close(udpSocket->fileDescriptor);
	udpSocket->fileDescriptor = -1;
	memset(udpSocket->buffer, 0, BUFFER_SIZE);
}

void sendToUDPSocket(UDPSocket *udpSocket)
{
	if (sendto(udpSocket->fileDescriptor,
	           udpSocket->buffer,
	           strnlen(udpSocket->buffer, BUFFER_SIZE),
	           0,
	           (struct sockaddr *) &udpSocket->address,
	           sizeof(udpSocket->address))
	    < 0) {
		error("Error sendto");
	}
}

void clearUDPSocketBuffer(UDPSocket *udpSocket)
{
	writeToUDPSocketBuffer(udpSocket, "");
}

void writeToUDPSocketBuffer(UDPSocket *udpSocket, const char *const string)
{
	if (string == NULL || string == udpSocket->buffer) {
		return;
	}

	strncpy(udpSocket->buffer, string, BUFFER_SIZE);
}

void sprintfToUDPSocketBuffer(UDPSocket *udpSocket,
                              const char *const format,
                              ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(udpSocket->buffer, BUFFER_SIZE, format, args);
	va_end(args);
}

void writeToUDPSocket(UDPSocket *udpSocket, const char *const string)
{
	writeToUDPSocketBuffer(udpSocket, string);

	write(udpSocket->fileDescriptor,
	      udpSocket->buffer,
	      strnlen(udpSocket->buffer, BUFFER_SIZE));
}
