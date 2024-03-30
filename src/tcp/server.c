#include "tcp/server.h"

#include "debug.h"
#include "tcp/socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

TCPSocket createListeningTCPSocket(const char *const listeningIPAddress,
                                   const int listeningPort)
{
	TCPSocket tcpSocket = {
	    .fileDescriptor = -1,
	    .address = createSocketAddress(listeningIPAddress, listeningPort),
	    .buffer  = {[0] = '\0', [BUFFER_SIZE] = '\0'},
	};

	if ((tcpSocket.fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error("Error opening socket\n");
	}

	if (bind(tcpSocket.fileDescriptor,
	         (struct sockaddr *) &tcpSocket.address,
	         sizeof(tcpSocket.address))
	    < 0) {
		error("Error binding socket\n");
	}

	if (listen(tcpSocket.fileDescriptor, 0) < 0) {
		error("Error listening for socket connections!\n");
	}

	return tcpSocket;
}
