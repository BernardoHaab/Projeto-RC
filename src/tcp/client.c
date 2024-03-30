#include "tcp/client.h"

#include "debug.h"
#include "tcp/socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

TCPSocket createConnectedTCPSocket(const char *const connectingIPAddress,
                                   const int connectingPort)
{
	TCPSocket tcpSocket = {
	    .fileDescriptor = -1,
	    .address = createSocketAddress(connectingIPAddress, connectingPort),
	    .buffer  = {[0] = '\0', [BUFFER_SIZE] = '\0'},
	};

	if ((tcpSocket.fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error("Error opening socket\n");
	}

	if (connect(tcpSocket.fileDescriptor,
	            (struct sockaddr *) &tcpSocket.address,
	            sizeof(tcpSocket.address))
	    < 0) {
		error("");
	}

	return tcpSocket;
}
