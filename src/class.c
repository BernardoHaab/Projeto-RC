#include "class.h"

#include "debug.h"
#include "tcp/socket.h"

#include <stdio.h>

void setupClass(TCPSocket *tcpSocket)
{
	debugMessage(stdout, OK, "Started Class Console\n");

	debugMessage(stdout, OK, "Ended Class Console\n");
}
