#include "client.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>

void clientHelpInputProcessing(const ClientCommand command)
{
	const char *const baseCommand = *(char **) vectorGet(&command.args, 0);
	if (baseCommand != NULL && *baseCommand != '\0'
	    && strcmp(baseCommand, "help") != 0) {
		printf("Invalid command\n");
	}
	printf("Available commands:\n");

#define CLIENT(ENUM,                \
               COMMAND,             \
               USAGE,               \
               ARGS_MIN,            \
               ARGS_MAX,            \
               INPUT_PROCESSING,    \
               RESPONSE_PROCESSING) \
	printf("- %s\n", USAGE);
	CLIENT_COMMANDS
#undef CLIENT
}

void clientLoginInputProcessing(const ClientCommand command) {}
void clientListClassesInputProcessing(const ClientCommand command) {}
void clientListSubscribedInputProcessing(const ClientCommand command) {}
void clientSubscribeInputProcessing(const ClientCommand command) {}
void clientCreateClassInputProcessing(const ClientCommand command) {}
void clientSendInputProcessing(const ClientCommand command) {}
