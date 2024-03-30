#include "server.h"

#include "admin.h"
#include "class.h"
#include "tcp/server.h"
#include "tcp/socket.h"
#include "udp/server.h"
#include "udp/socket.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *usersFilepath = NULL;

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

	TCPSocket classSocket  = createListeningTCPSocket(ipAddress, portClass);
	UDPSocket configSocket = createBindedUDPSocket(ipAddress, portConfig);

	pid_t adminPid;
	if ((adminPid = fork()) == 0) {
		setupAdminConsole(&configSocket);
		exit(0);
	} else if (adminPid < 0) {
		perror("Error forking for setupAdminConsole");
		exit(EXIT_FAILURE);
	}

	pid_t classPid;
	if ((classPid = fork()) == 0) {
		setupClass(&classSocket);
		exit(0);
	} else if (classPid < 0) {
		perror("Error forking for setupClass");
		exit(EXIT_FAILURE);
	}

	if (waitpid(adminPid, NULL, 0) == -1) {
		perror("Error waiting for adminPid");
	}

	if (kill(classPid, SIGTERM) == -1) {
		perror("Error sending SIGTERM to child process for setupClass");
		if (kill(classPid, SIGKILL) == -1) {
			perror("Error sending SIGKILL to child process for "
			       "setupClass");
		}
	}

	return 0;
}

void usage(const char *const programName)
{
	printf(
	    "Usage: %s [options] <port classes> <port config> <config file>\n",
	    programName);
	printf("Options:\n");
	printf("  -h, --help                   Print this usage message\n");

	exit(EXIT_FAILURE);
}
