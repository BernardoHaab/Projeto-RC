#include "client.h"

#include "command.h"
#include "tcp/client.h"
#include "tcp/socket.h"
#include "udp/socket.h"
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

	readFromTCPSocket(&connectionTCPSocket);

	loop
	{
		printf("%s\n", trim(connectionTCPSocket.buffer));

		ClientCommand command = {0};

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

			command
			    = parseClientCommand(connectionTCPSocket.buffer);

			// TODO: Parse if the command is valid
			validCommand = true;
		}

		strcat(connectionTCPSocket.buffer, "\n");

		writeToTCPSocket(&connectionTCPSocket, NULL);

		readFromTCPSocket(&connectionTCPSocket);

		command.responseProcessing(connectionTCPSocket.buffer);
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

ClientCommand parseClientCommand(const char *const string)
{
	ClientCommand command = {0};

	command.args = vectorStringSplit(string, CLI_COMMAND_DELIMITER);
	command.command
	    = parseClientCommandType(*(char **) vectorGet(&command.args, 0));

	switch (command.command) {
#define CLIENT(ENUM,                                              \
               COMMAND,                                           \
               USAGE,                                             \
               ARGS_MIN,                                          \
               ARGS_MAX,                                          \
               INPUT_PROCESSING,                                  \
               RESPONSE_PROCESSING)                               \
	case ENUM: {                                              \
		command.inputProcessing    = INPUT_PROCESSING;    \
		command.responseProcessing = RESPONSE_PROCESSING; \
	} break;
		CLIENT_COMMANDS
#undef CLIENT
	}

	return command;
}

void printClientCommand(FILE *file, const ClientCommand command)
{
	fprintf(file,
	        CLIENT_COMMAND_FORMAT "\n",
	        clientCommandTypeToString(command.command),
	        vectorString(command.args));
}

ClientCommandType parseClientCommandType(const char *const string)
{
	if (string == NULL || *string == '\0') {
		return CLIENT_HELP;
	}

#define CLIENT(ENUM,                         \
               COMMAND,                      \
               USAGE,                        \
               ARGS_MIN,                     \
               ARGS_MAX,                     \
               INPUT_PROCESSING,             \
               RESPONSE_PROCESSING)          \
	if (strcmp(string, #COMMAND) == 0) { \
		return ENUM;                 \
	}
	CLIENT_COMMANDS
#undef CLIENT

	return CLIENT_HELP;
}

char *clientCommandTypeToString(const ClientCommandType command)
{
	switch (command) {
#define CLIENT(ENUM,                \
               COMMAND,             \
               USAGE,               \
               ARGS_MIN,            \
               ARGS_MAX,            \
               INPUT_PROCESSING,    \
               RESPONSE_PROCESSING) \
	case ENUM:                  \
		return #ENUM;
		CLIENT_COMMANDS
#undef CLIENT
	}

	return NULL;
}

void *clientMultiCastThread(void *argument)
{
	const char *const multicastIPString = (char *) argument;

#ifndef DEBUG
	printf("Thread starting for multicast IP: %s...\n", multicastIPString);
#endif

	pthread_exit(NULL);

	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in addr = {0};
	addr.sin_family         = AF_INET;
	addr.sin_addr.s_addr    = inet_addr(multicastIPString);
	addr.sin_port           = htons(5151);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind: ");
		pthread_exit(NULL);
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastIPString);
	mreq.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))
	    < 0) {
		perror("setsockopt: ");
		pthread_exit(NULL);
	}

	int nbytes;
	unsigned int addrlen;
	char msg[256] = {0};
	while (1) {
		if ((nbytes = recvfrom(sock,
		                       msg,
		                       sizeof(msg) - 1,
		                       0,
		                       (struct sockaddr *) &addr,
		                       &addrlen))
		    < 0) {
			perror("recvfrom: ");
			pthread_exit(NULL);
		}

		printf("Received multicast message: %s\n", msg);
	}

	if (setsockopt(sock,
	               IPPROTO_IP,
	               IP_DROP_MEMBERSHIP,
	               &mreq,
	               sizeof(mreq))
	    < 0) {
		perror("setsockopt: ");
		pthread_exit(NULL);
	}
	close(sock);


	pthread_exit(NULL);
}
