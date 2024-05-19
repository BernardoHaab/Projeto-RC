#include "tcp/socket.h"

static void classDefaultPostReceiveHook(const TCPSocket socket)
{
	(void) socket;
}

void classHelpPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classLoginPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classListClassesPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classListSubscribedPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classSubscribePostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classCreateClassPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}

void classSendPostReceiveHook(const TCPSocket socket)
{
	classDefaultPostReceiveHook(socket);
}
