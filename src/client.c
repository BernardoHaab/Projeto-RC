#include "client.h"

#include "command/class.h"
#include "command/client.h"
#include "tcp/client.h"
#include "tcp/socket.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

	ClientCommandOptional commandOpt = {0};
	loop
	{
		readFromTCPSocket(&connectionTCPSocket);

		ClientCommand command = commandOpt.command;

		if (command.postReceiveHook != NULL) {
			command.postReceiveHook(connectionTCPSocket);
		} else {
			printf("%s\n", connectionTCPSocket.buffer);
		}

		bool validCommand = false;
		while (!validCommand) {
			printf(PROMPT);
			if (scanf(" %[^\n]%*c", connectionTCPSocket.buffer)
			        == EOF
			    || strncmp(connectionTCPSocket.buffer,
			               EXIT_COMMAND,
			               sizeof(EXIT_COMMAND))
			           == 0) {
				exit(EXIT_FAILURE);
			}

			commandOpt
			    = parseClientCommand(connectionTCPSocket.buffer);
			if (!commandOpt.valid) {
				continue;
			}

			command = commandOpt.command;

			if (command.command == COMMAND_HELP) {
				command.preSendHook(commandOpt.command,
				                    &connectionTCPSocket);
				continue;
			}

			validCommand = true;
		}

		command.preSendHook(commandOpt.command, &connectionTCPSocket);

		writeToTCPSocket(&connectionTCPSocket, NULL);
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

void *clientMultiCastThread(void *argument)
{
#define EXIT_THREAD(MESSAGE)                \
	do {                                \
		perror(MESSAGE);            \
		vectorClear(args);          \
		pthread_exit(NULL);         \
	} while (0);

	Vector *args = (Vector *) argument;

	const char *const multicastIPString = *(char **) vectorGet(args, 1);

#ifndef DEBUG
	vectorPrint(stdout, *args);
	printf("Thread starting for multicast IP: %s...\n", multicastIPString);
#endif

	//	pthread_exit(NULL);

	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		EXIT_THREAD("socket: ");
	}

	struct sockaddr_in addr = {0};
	addr.sin_family         = AF_INET;
	addr.sin_addr.s_addr    = inet_addr(multicastIPString);
	addr.sin_port           = htons(5000);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		EXIT_THREAD("bind: ");
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastIPString);
	mreq.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))
	    < 0) {
		EXIT_THREAD("setsockopt: ");
	}

	int nbytes;
	unsigned int addrlen;
	char msg[256] = {0};
	while (1) {
		memset(msg, 0, sizeof(msg) - 1);
		if ((nbytes = recvfrom(sock,
		                       msg,
		                       sizeof(msg) - 1,
		                       0,
		                       (struct sockaddr *) &addr,
		                       &addrlen))
		    < 0) {
			EXIT_THREAD("recvfrom: ");
		}

		printf("Received multicast message: %s\n", msg);
	}

	if (setsockopt(sock,
	               IPPROTO_IP,
	               IP_DROP_MEMBERSHIP,
	               &mreq,
	               sizeof(mreq))
	    < 0) {
		EXIT_THREAD("setsockopt: ");
	}
	close(sock);


	vectorClear(args);
	pthread_exit(NULL);
#undef EXIT_THREAD
}
