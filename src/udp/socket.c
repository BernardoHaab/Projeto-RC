#include "udp/socket.h"

#include "debug.h"

#include <netinet/in.h>
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
		error("Error receiving from UDP Socket!\n");
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
