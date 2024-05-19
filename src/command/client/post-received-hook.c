#include "client.h"
#include "command.h"
#include "tcp/socket.h"
#include "vector.h"

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static void clientDefaultPostReceiveHook(const TCPSocket socket)
{
	printf("%s\n", socket.buffer);
}

void clientHelpPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}

void clientLoginPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}

void clientListClassesPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}

void clientListSubscribedPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}

void clientSubscribePostReceiveHook(const TCPSocket socket)
{
	Vector args = vectorStringSplit(socket.buffer, COMMAND_DELIMITER "/:");

	if (args.size != 2
	    || strcmp(*(char **) vectorGet(&args, 1), COMMAND_REPLY_REJECTED)
	           == 0) {
		clientDefaultPostReceiveHook(socket);
		return;
	}

	pthread_t thread;
	pthread_create(&thread, NULL, clientMultiCastThread, &args);

	clientDefaultPostReceiveHook(socket);
}

void clientCreateClassPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}

void clientSendPostReceiveHook(const TCPSocket socket)
{
	clientDefaultPostReceiveHook(socket);
}
