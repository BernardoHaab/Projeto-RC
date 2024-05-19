#include "tcp/socket.h"

#include <assert.h>
#include <stdio.h>

static void clientDefaultPostReceiveHook(const TCPSocket socket)
{
	printf("%s\n", socket.buffer);
}

void clientHelpPostReceiveHook(const TCPSocket socket)
{
	assert(0 && "TODO: Not yet implemented\n");
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
