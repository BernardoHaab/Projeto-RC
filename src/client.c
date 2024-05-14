#include "client.h"

#include "command.h"
#include "tcp/client.h"
#include "tcp/socket.h"
#include "udp/socket.h"
#include "utils.h"
#include "vector.h"

#include <bits/types/struct_iovec.h>
#include <stdbool.h>
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

			const ClientCommand command
			    = parseClientCommand(connectionTCPSocket.buffer);
			printClientCommand(stdout, command);

			// TODO: Parse if the command is valid
			validCommand = true;
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

ClientCommand parseClientCommand(const char *const string)
{
	ClientCommand command = {0};

	command.args    = vectorStringSplit(string, CLI_COMMAND_DELIMITER);
	command.command = parseClientCommandType(vectorGet(&command.args, 0));

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

	return CLIENT_HELP;
}

char *clientCommandTypeToString(const ClientCommandType command)
{
	switch (command) {
#define CLIENT(ENUM,                \
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
