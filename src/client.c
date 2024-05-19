#include "client.h"

#include "command/class.h"
#include "command/client.h"
#include "tcp/client.h"
#include "tcp/socket.h"
#include "utils.h"

#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

sem_t promptSemaphore;
static void initializeSemaphore(void);
static void terminateSemaphore(void);

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

	initializeSemaphore();

	const char *const serverIPAddress = argv[1];
	const int classPort               = atoi(argv[2]);

	TCPSocket connectionTCPSocket
	    = createConnectedTCPSocket(serverIPAddress, classPort);

	ClientCommandOptional commandOpt = {0};
	loop
	{
		readFromTCPSocket(&connectionTCPSocket);

		ClientCommand command = commandOpt.command;

		if (command.postReceiveHook != NULL) {
			command.postReceiveHook(connectionTCPSocket);
		} else {
			printf("%s\n", connectionTCPSocket.buffer);
		}

		bool validCommand = false;
		while (!validCommand) {
			sem_wait(&promptSemaphore);
			printf(PROMPT);
			if (scanf(" %[^\n]%*c", connectionTCPSocket.buffer)
			        == EOF
			    || strncmp(connectionTCPSocket.buffer,
			               EXIT_COMMAND,
			               sizeof(EXIT_COMMAND))
			           == 0) {
				exit(EXIT_FAILURE);
			}
			sem_post(&promptSemaphore);

			commandOpt
			    = parseClientCommand(connectionTCPSocket.buffer);
			if (!commandOpt.valid) {
				continue;
			}

			command = commandOpt.command;

			if (command.command == COMMAND_HELP) {
				command.preSendHook(commandOpt.command,
				                    &connectionTCPSocket);
				continue;
			}

			validCommand = true;
		}

		command.preSendHook(commandOpt.command, &connectionTCPSocket);

		writeToTCPSocket(&connectionTCPSocket, NULL);
	}

	closeTCPSocket(&connectionTCPSocket);

	terminateSemaphore();

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

void *clientMultiCastThread(void *argument)
{
	const char *const multicastIPString = (char *) argument;

#ifndef DEBUG
	printf("Thread starting for multicast IP: %s...\n", multicastIPString);
#endif

	sem_post(&promptSemaphore);

	sem_wait(&promptSemaphore);

	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		sem_post(&promptSemaphore);
		perror("socket");
		pthread_exit(NULL);
	}

	struct sockaddr_in addr = {0};
	addr.sin_family         = AF_INET;
	addr.sin_addr.s_addr    = inet_addr(multicastIPString);
	addr.sin_port           = htons(5000);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		sem_post(&promptSemaphore);
		perror("bind: ");
		pthread_exit(NULL);
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastIPString);
	mreq.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))
	    < 0) {
		sem_post(&promptSemaphore);
		perror("setsockopt: ");
		pthread_exit(NULL);
	}

	sem_post(&promptSemaphore);

	int nbytes;
	unsigned int addrlen;
	char msg[256] = {0};
	while (1) {
		memset(msg, 0, sizeof(msg));
		if ((nbytes = recvfrom(sock,
		                       msg,
		                       sizeof(msg) - 1,
		                       0,
		                       (struct sockaddr *) &addr,
		                       &addrlen))
		    < 0) {
			sem_post(&promptSemaphore);
			perror("recvfrom: ");
			pthread_exit(NULL);
		}

		sem_wait(&promptSemaphore);
		printf("Received multicast message: %s\n", msg);
		sem_post(&promptSemaphore);
	}

	if (setsockopt(sock,
	               IPPROTO_IP,
	               IP_DROP_MEMBERSHIP,
	               &mreq,
	               sizeof(mreq))
	    < 0) {
		sem_post(&promptSemaphore);
		perror("setsockopt: ");
		pthread_exit(NULL);
	}
	close(sock);


	pthread_exit(NULL);
}

static void initializeSemaphore(void)
{
	if (sem_init(&promptSemaphore, 0, 1) == -1) {
		perror("sem_init: ");
	}
}

static void terminateSemaphore(void)
{
	sem_destroy(&promptSemaphore);
}
