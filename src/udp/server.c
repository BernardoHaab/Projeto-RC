#include "udp/server.h"

#include "debug.h"
#include "udp/socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

UDPSocket createBindedUDPSocket(const char *const bindingIPAddress,
                                const int bindingPort)
{
	UDPSocket udpSocket = {0};

	udpSocket.address.sin_family      = AF_INET;
	udpSocket.address.sin_port        = htons(bindingPort);
	udpSocket.address.sin_addr.s_addr = inet_addr(bindingIPAddress);

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
