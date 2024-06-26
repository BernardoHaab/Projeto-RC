#include "tcp/socket.h"

#include "debug.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct sockaddr_in createSocketAddress(const char *const ipAddress,
                                       const int port)
{
	struct sockaddr_in socket = {0};

	socket.sin_family      = AF_INET;
	socket.sin_addr.s_addr = inet_addr(ipAddress);
	socket.sin_port        = htons(port);

	return socket;
}

TCPSocket createEmptyTCPSocket(void)
{
	return (TCPSocket){
	    .fileDescriptor = -1,
	    .buffer         = {0},
	    .address        = {0},
	};
}

ssize_t readFromTCPSocket(TCPSocket *connectingTCPSocket)
{
	ssize_t receivedBytes = read(connectingTCPSocket->fileDescriptor,
	                             connectingTCPSocket->buffer,
	                             BUFFER_SIZE);

	if (receivedBytes < 0) {
		error("read: ");
	}

	connectingTCPSocket->buffer[receivedBytes] = '\0';

	return receivedBytes;
}

void clearTCPSocketBuffer(TCPSocket *tcpSocket)
{
	writeToTCPSocketBuffer(tcpSocket, "");
}

void writeToTCPSocketBuffer(TCPSocket *tcpSocket, const char *const string)
{
	if (string == NULL || string == tcpSocket->buffer) {
		return;
	}

	strncpy(tcpSocket->buffer, string, BUFFER_SIZE);
}

void sprintfToTCPSocketBuffer(TCPSocket *tcpSocket,
                              const char *const format,
                              ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(tcpSocket->buffer, BUFFER_SIZE, format, args);
	va_end(args);
}

void writeToTCPSocket(TCPSocket *tcpSocket, const char *const string)
{
	writeToTCPSocketBuffer(tcpSocket, string);

	write(tcpSocket->fileDescriptor,
	      tcpSocket->buffer,
	      strnlen(tcpSocket->buffer, BUFFER_SIZE));
}

void closeTCPSocket(TCPSocket *tcpSocket)
{
	close(tcpSocket->fileDescriptor);
	tcpSocket->fileDescriptor = -1;
	memset(tcpSocket->buffer, 0, BUFFER_SIZE);
}
