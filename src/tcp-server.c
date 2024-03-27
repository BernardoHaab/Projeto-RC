#include "tcp-server.h"

#include "debug.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

TCPSocket createTCPSocket(const char *const ipAddress, const int port)
{
	TCPSocket tcpSocket = {
	    .fileDescriptor = -1,
	    .address        = createSocketAddress(ipAddress, port),
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

struct sockaddr_in createSocketAddress(const char *const ipAddress,
                                       const int port)
{
	struct sockaddr_in socket = {0};

	socket.sin_family      = AF_INET;
	socket.sin_addr.s_addr = inet_addr(ipAddress);
	socket.sin_port        = htons(port);

	return socket;
}
