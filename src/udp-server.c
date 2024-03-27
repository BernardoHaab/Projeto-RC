#include "udp-server.h"

#include "debug.h"
#include "utils.h"

#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>

UDPSocket createUDPSocket(const char *const ipAddress, const int port)
{
	UDPSocket udpSocket = {0};

	udpSocket.address.sin_family      = AF_INET;
	udpSocket.address.sin_port        = htons(port);
	udpSocket.address.sin_addr.s_addr = inet_addr(ipAddress);

	if ((udpSocket.fileDescriptor
	     = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
	    == -1) {
		error("Erro na criação do socket");
	}

	if (bind(udpSocket.fileDescriptor,
	         (struct sockaddr *) &udpSocket.address,
	         sizeof(udpSocket.address))
	    == -1) {
		error("Error binding socket\n");
	}

	udpSocket.buffer[0]           = '\0';
	udpSocket.buffer[BUFFER_SIZE] = '\0';

	return udpSocket;
}

struct sockaddr_in receiveFromUDPSocket(UDPSocket *udpSocket)
{
	int receivedBytes                  = -1;
	struct sockaddr_in clientIPAddress = {0};
	socklen_t clientIPAddressLength    = sizeof(clientIPAddress);

	if ((receivedBytes = recvfrom(udpSocket->fileDescriptor,
	                              udpSocket->buffer,
	                              BUFFER_SIZE,
	                              0,
	                              (struct sockaddr *) &clientIPAddress,
	                              &clientIPAddressLength))
	    == -1) {
		error("Erro no recvfrom");

		return (struct sockaddr_in){0};
	}

	udpSocket->buffer[receivedBytes] = '\0';
	trim(udpSocket->buffer);

	return clientIPAddress;
}
