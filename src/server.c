#include "server.h"

#include "admin.h"
#include "tcp-server.h"
#include "udp-server.h"

#include <getopt.h>
#include <netinet/in.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVER_PORT_CLASS_DEFAULT  9494
#define SERVER_PORT_CONFIG_DEFAULT 6767
char *usersFilepath = NULL;

void usage(const char *const programName)
{
	printf(
	    "Usage: %s [options] <port classes> <port config> <config file>\n",
	    programName);
	printf("Options:\n");
	printf("  -h, --help                   Print this usage message\n");

	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	char *ipAddress      = SERVER_IP_DEFAULT;
	int portClass        = SERVER_PORT_CLASS_DEFAULT;
	int portConfig       = SERVER_PORT_CONFIG_DEFAULT;
	char *configFilepath = NULL;

	pid_t wpid;
	int status = 0;

	if (argc != 4) {
		usage(argv[0]);
	}

	portClass      = atoi(argv[1]);
	portConfig     = atoi(argv[2]);
	usersFilepath  = argv[3];

	const TCPSocket classSocket = createTCPSocket(ipAddress, portClass);
	UDPSocket configSocket      = createUDPSocket(ipAddress, portConfig);

	if (fork() == 0) {
		setupAdminConsole(&configSocket);
		exit(0);
	}

	if (fork() == 0) {
		// TODO: setup class server
		exit(0);
	}

	while ((wpid = wait(&status)) > 0)
		; // this way, the father waits for all the child

	return 0;
}
