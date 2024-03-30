#include "class.h"

#include "debug.h"
#include "tcp/server.h"
#include "tcp/socket.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void setupClass(TCPSocket *serverTCPSocket)
{
	debugMessage(stdout, OK, "Started Class Console\n");

	loop
	{
		// NOTE: Clean finished child process to avoid zombies
		while (waitpid(-1, NULL, WNOHANG) > 0) {};

		TCPSocket clientTCPSocket
		    = acceptFromTCPSocket(*serverTCPSocket);
		if (clientTCPSocket.fileDescriptor <= 0) {
			continue;
		}

		/* debugMessage(stdout, OK, "New Connection with client: "); */
		/* printSocketIP(stdout, true, clientIPAddress); */

		if (fork() == 0) {
			closeTCPSocket(serverTCPSocket);
			processClient(clientTCPSocket);
			exit(EXIT_SUCCESS);
		}

		closeTCPSocket(&clientTCPSocket);
	}

	debugMessage(stdout, OK, "Ended Class Console\n");
}

void processClient(TCPSocket clientTCPSocket)
{
	do {
		const int nRead = readFromTCPSocket(&clientTCPSocket);
		if (nRead <= 0) {
			break;
		}

		// TODO: Add an abstract api to print this debug information
		char ipAddressString[INET_ADDRSTRLEN] = {0};
		printf("[" RED "%s" RESET ":" CYAN "%-6hu" RESET
		       "] Received " BLUE "%4d" RESET " bytes: %s",
		       inet_ntop(AF_INET,
		                 &clientTCPSocket.address.sin_addr,
		                 ipAddressString,
		                 INET_ADDRSTRLEN),
		       clientTCPSocket.address.sin_port,
		       nRead,
		       clientTCPSocket.buffer);

		/* writeToTCPSocketBuffer(&clientTCPSocket, */
		/*                        "received your message"); */

		writeToTCPSocket(&clientTCPSocket);
	} while (true);
}
