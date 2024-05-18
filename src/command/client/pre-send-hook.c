#include "client.h"
#include "command/client.h"
#include "tcp/socket.h"
#include "vector.h"

#include <assert.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern sem_t promptSemaphore;

void clientHelpPreSendHook(const ClientCommand command, TCPSocket *socket)
{
	const char *const baseCommand = *(char **) vectorGet(&command.args, 0);
	if (baseCommand != NULL && *baseCommand != '\0'
	    && strcmp(baseCommand, "help") != 0) {
		printf("Invalid command\n");
	}
	printf("Available commands:\n");

#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	printf("- %s %s\n", #BASE_COMMAND, USAGE_ARGS);
	CLIENT_COMMANDS
#undef COMMAND
}

void clientLoginPreSendHook(const ClientCommand command, TCPSocket *socket) {}

void clientListClassesPreSendHook(const ClientCommand command,
                                  TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void clientListSubscribedPreSendHook(const ClientCommand command,
                                     TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void clientSubscribePreSendHook(const ClientCommand command, TCPSocket *socket)
{
	Vector args = vectorStringSplit(socket->buffer, " ");

	pthread_t thread;
	sem_wait(&promptSemaphore);
	pthread_create(&thread,
	               NULL,
	               clientMultiCastThread,
	               *(char **) vectorGet(&args, 1));
}

void clientCreateClassPreSendHook(const ClientCommand command,
                                  TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void clientSendPreSendHook(const ClientCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}
