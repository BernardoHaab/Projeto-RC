#include "server.h"

#include "admin.h"
#include "class.h"
#include "tcp-server.h"
#include "udp-server.h"

#include <getopt.h>
#include <netinet/in.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
	char *ipAddress = SERVER_IP_DEFAULT;

	pid_t wpid;
	int status = 0;

	for (int i = 0; i < argc; ++i) {
		const char *const arg = argv[i];
		if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
			usage(argv[0]);
		}
	}

	if (argc != 4) {
		usage(argv[0]);
	}

	const int portClass  = atoi(argv[1]);
	const int portConfig = atoi(argv[2]);
	usersFilepath        = argv[3];

	TCPSocket classSocket  = createTCPSocket(ipAddress, portClass);
	UDPSocket configSocket = createUDPSocket(ipAddress, portConfig);

	if (fork() == 0) {
		setupAdminConsole(&configSocket);
		exit(0);
	}

	if (fork() == 0) {
		setupClass(&classSocket);
		exit(0);
	}

	while ((wpid = wait(&status)) > 0)
		; // this way, the father waits for all the child

	return 0;
}
