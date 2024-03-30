#include "client.h"

#include "tcp/client.h"
#include "tcp/socket.h"
#include "utils.h"

#include <bits/types/struct_iovec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	for (int i = 0; i < argc; ++i) {
		const char *const arg = argv[i];
		if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
			usage(argv[0]);
		}
	}

	if (argc != 3) {
		usage(argv[0]);
	}

	const char *const serverIPAddress = argv[1];
	const int classPort               = atoi(argv[2]);

	TCPSocket connectionTCPSocket
	    = createConnectedTCPSocket(serverIPAddress, classPort);

	loop
	{
		readFromTCPSocket(&connectionTCPSocket);

		printf("%s\n", trim(connectionTCPSocket.buffer));

		printf(PROMPT);
		if (scanf(" %[^\n]%*c", connectionTCPSocket.buffer) == EOF
		    || strncmp(connectionTCPSocket.buffer,
		               EXIT_COMMAND,
		               sizeof(EXIT_COMMAND))
		           == 0) {
			break;
		}

		strncpy(connectionTCPSocket.buffer
		            + strnlen(connectionTCPSocket.buffer, BUFFER_SIZE),
		        "\n",
		        BUFFER_SIZE);

		writeToTCPSocket(&connectionTCPSocket);
	}

	closeTCPSocket(&connectionTCPSocket);

	return 0;
}

void usage(const char *const programName)
{
	printf("Usage: %s [options] <endereço do servidor> <class port>\n",
	       programName);
	printf("Options:\n");
	printf("  -h, --help                   Print this usage message\n");

	exit(EXIT_FAILURE);
}
