#include "class.h"

#include "command.h"
#include "command/class.h"
#include "debug.h"
#include "tcp/server.h"
#include "tcp/socket.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

Vector loggedClients = {0};

static void
printLoggedClient(FILE *file, const void *const data, const size_t size);

void setupClass(TCPSocket *serverTCPSocket)
{
	loggedClients
	    = vectorCreate(sizeof(LoggedClient), NULL, printLoggedClient);

	debugMessage(stdout, OK, "Started Class Console\n");

	resetClasses();

	loop
	{
		// NOTE: Clean finished child process to avoid zombies
		while (waitpid(-1, NULL, WNOHANG) > 0) {};

		TCPSocket clientTCPSocket
		    = acceptFromTCPSocket(*serverTCPSocket);
		if (clientTCPSocket.fileDescriptor <= 0) {
			continue;
		}

#if DEBUG
		debugMessage(stdout, OK, "New Connection with client: ");
		printSocketIP(stdout, true, clientIPAddress);
#endif

		if (fork() == 0) {
			closeTCPSocket(serverTCPSocket);
			processClient(clientTCPSocket);
			exit(EXIT_SUCCESS);
		}

		closeTCPSocket(&clientTCPSocket);
	}

	debugMessage(stdout, OK, "Ended Class Console\n");
}

void processClient(TCPSocket clientTCPSocket)
{
	writeToTCPSocket(&clientTCPSocket, MOTD);

	ClassCommand command = {0};
	do {
		const int nRead = readFromTCPSocket(&clientTCPSocket);
		if (nRead <= 0) {
			break;
		}

		// TODO: Add an abstract api to print this debug information
		char ipAddressString[INET_ADDRSTRLEN] = {0};
		printf("[" RED "%s" RESET ":" CYAN "%-6hu" RESET
		       "] Received " BLUE "%4d" RESET " bytes: %s\n",
		       inet_ntop(AF_INET,
		                 &clientTCPSocket.address.sin_addr,
		                 ipAddressString,
		                 INET_ADDRSTRLEN),
		       clientTCPSocket.address.sin_port,
		       nRead,
		       clientTCPSocket.buffer);

		const ClassCommandOptional commandOpt
		    = parseClassCommand(clientTCPSocket.buffer);

		command = commandOpt.command;

		if (!commandOpt.valid) {
			destroyClassCommand(&command);
			continue;
		}

		command.postReceiveHook(clientTCPSocket);
		command.preSendHook(command, &clientTCPSocket);

		/* globalCurrentReqIP = clientTCPSocket.address; */
		/* processClassCommand(cliCommand, */
		/*                     isClientLogged(globalCurrentReqIP), */
		/*                     clientTCPSocket.buffer, */
		/*                     BUFFER_SIZE); */

		writeToTCPSocket(&clientTCPSocket, NULL);
		fflush(stdout);
	} while (true);
}

bool isClientLogged(const Vector loggedClients,
                    const struct sockaddr_in clientIP)
{
	for (size_t i = 0; i < loggedClients.size; ++i) {
		const LoggedClient currentLoggedClient
		    = *(LoggedClient *) vectorGet(&loggedClients, i);

		if (isSameAddress(currentLoggedClient.address, clientIP)) {
			return true;
		}
	}

	return false;
}

bool isSameAddress(const struct sockaddr_in a, const struct sockaddr_in b)
{
	const bool isSameIp   = a.sin_addr.s_addr == b.sin_addr.s_addr;
	const bool isSamePort = a.sin_port == b.sin_port;

	return isSameIp && isSamePort;
}

struct sockaddr_in createNewIpMultiCast()
{
	int shmFd
	    = shm_open(MULTICAST_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	ftruncate(shmFd, sizeof(int));
	int *lastUsedIp = (int *) mmap(NULL,
	                               sizeof(int),
	                               PROT_READ | PROT_WRITE,
	                               MAP_SHARED,
	                               shmFd,
	                               0);

	printf("lastUsedIp: %d\n", *lastUsedIp);

	*lastUsedIp = *lastUsedIp + 1;

	char ip[12] = {0};
	sprintf(ip, "239.0.0.%d", *lastUsedIp);

	struct sockaddr_in ipMulticast = {0};

	memset(&ipMulticast, 0, sizeof(ipMulticast));

	ipMulticast.sin_family      = AF_INET;
	ipMulticast.sin_addr.s_addr = inet_addr(ip);
	ipMulticast.sin_port        = htons(5000);

	munmap(lastUsedIp, sizeof(int));
	close(shmFd);

	return ipMulticast;
}

void resetClasses()
{
	int shmFd
	    = shm_open(CLASSES_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	ftruncate(shmFd, sizeof(Class) * CLASS_MAX_SIZE);
	Class *classes_ptr = (Class *) mmap(NULL,
	                                    sizeof(Class) * CLASS_MAX_SIZE,
	                                    PROT_READ | PROT_WRITE,
	                                    MAP_SHARED,
	                                    shmFd,
	                                    0);

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		classes_ptr[i] = (Class){0};
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);
}

void addClientLogin(Vector *loggedClients,
                    const struct sockaddr_in clientIP,
                    const Role role)
{
	if (loggedClients == NULL) {
		return;
	}

	// WARN: This function in not checking for duplicates

	vectorPushBack(loggedClients,
	               &(LoggedClient){
	                   .address = clientIP,
	                   .role    = role,
	               });

#if DEBUG
	vectorPrint(loggedClients);
#endif
}


void getFormatedIp(struct sockaddr_in ip, char *buffer)
{
	sprintf(buffer, "%s:%d", inet_ntoa(ip.sin_addr), ip.sin_port);
}

LoggedClient *getLoggedClient(const Vector loggedClients,
                              const struct sockaddr_in clientIP)
{
	for (size_t i = 0; i < loggedClients.size; ++i) {
		LoggedClient *currentLoggedClient
		    = (LoggedClient *) vectorGet(&loggedClients, i);

		if (isSameAddress(currentLoggedClient->address, clientIP)) {
			return currentLoggedClient;
		}
	}

	return NULL;
}

Role getLoggedClientRole(const Vector loggedClients,
                         const struct sockaddr_in clientIP)
{
	LoggedClient *loggedClient = getLoggedClient(loggedClients, clientIP);

	return loggedClient != NULL ? loggedClient->role : ROLE_GUEST;
}

static void
printLoggedClient(FILE *file, const void *const data, const size_t size)
{
	(void) size;

	fprintf(file,
	        "%s",
	        inet_ntoa(((LoggedClient *) data)->address.sin_addr));
}
