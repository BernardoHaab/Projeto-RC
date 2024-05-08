#include "class.h"

#include "command.h"
#include "debug.h"
#include "server.h"
#include "tcp/server.h"
#include "tcp/socket.h"
#include "utils.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
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

struct LoggedClient loggedClients[CLIENT_LOGGED_MAX];
struct LoggedClient currentClient;
struct sockaddr_in globalCurrentReqIP;
// struct Class classes[MAX_CLASSES];

void setupClass(TCPSocket *serverTCPSocket)
{
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

		/* debugMessage(stdout, OK, "New Connection with client: "); */
		/* printSocketIP(stdout, true, clientIPAddress); */

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
	writeToTCPSocketBuffer(&clientTCPSocket, MOTD);
	writeToTCPSocket(&clientTCPSocket);

	CliCommand cliCommand = {0};

	do {
		const int nRead = readFromTCPSocket(&clientTCPSocket);
		if (nRead <= 0) {
			break;
		}

		// TODO: Add an abstract api to print this debug information
		char ipAddressString[INET_ADDRSTRLEN] = {0};
		printf("[" RED "%s" RESET ":" CYAN "%-6hu" RESET
		       "] Received " BLUE "%4d" RESET " bytes: %s",
		       inet_ntop(AF_INET,
		                 &clientTCPSocket.address.sin_addr,
		                 ipAddressString,
		                 INET_ADDRSTRLEN),
		       clientTCPSocket.address.sin_port,
		       nRead,
		       clientTCPSocket.buffer);

		parseCliCommand(trim((&clientTCPSocket)->buffer), &cliCommand);

		/* writeToTCPSocketBuffer(&clientTCPSocket, */
		/*                        "received your message"); */

		globalCurrentReqIP = clientTCPSocket.address;
		processClassCommand(cliCommand,
		                    isClientLogged(globalCurrentReqIP),
		                    clientTCPSocket.buffer,
		                    BUFFER_SIZE);

		writeToTCPSocket(&clientTCPSocket);
	} while (true);
}


ClassCommand processClassCommand(const CliCommand cliCommand,
                                 const bool isLoggedIn,
                                 char *responseBuffer,
                                 const size_t responseBufferSize)
{
	if (cliCommand.nargs <= 0) {
		strncpy(responseBuffer,
		        "Command not found. Tip: Type \"help\"\n",
		        responseBufferSize);
		return CLASS_HELP;
	}

	bool commandFound    = false;
	ClassCommand command = CLASS_HELP;
	do {
#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION)         \
	if (strcmp(cliCommand.args[0], COMMAND) == 0) { \
		commandFound = true;                    \
		command      = ENUM;                    \
		FUNCTION(cliCommand, responseBuffer);   \
		break;                                  \
	}
		CLASS_NOT_LOGGED_IN_COMMANDS
#undef WRAPPER
#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION)               \
	if (strcmp(cliCommand.args[0], COMMAND) == 0) {       \
		commandFound = true;                          \
		if (isLoggedIn) {                             \
			command = ENUM;                       \
			FUNCTION(cliCommand, responseBuffer); \
		}                                             \
		break;                                        \
	}
		CLASS_LOGGED_IN_COMMANDS
#undef WRAPPER
	} while (false);

	if (!commandFound) {
		strncpy(responseBuffer,
		        "Command not found. Tip: Type \"help\"\n",
		        responseBufferSize);
	}

	return command;
}


bool isClientLogged(const struct sockaddr_in clientIP)
{
	for (int i = 0; i < CLIENT_LOGGED_MAX; i++) {
		bool isSameIp = loggedClients[i].clientAddrs.sin_addr.s_addr
		                == clientIP.sin_addr.s_addr;
		bool isSamePort = loggedClients[i].clientAddrs.sin_port
		                  == clientIP.sin_port;

		if (isSameIp && isSamePort) {
			return true;
		}
	}

	return false;
}

void classLoginCommand(const CliCommand cliCommand, char *response)
{
	response[0] = '\0';

	if (usersFilepath == NULL) {
		return;
	}

	if (cliCommand.nargs != 3) {
		// TODO: usage
		return;
	}

	FILE *users = fopen(usersFilepath, "r");
	if (users == NULL) {
		debugMessage(stderr, ERROR, "Opening file %s\n", usersFilepath);
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		sprintf(response, "Error opening file %s\n", usersFilepath);
		return;
	}

	const char *const username = cliCommand.args[1];
	const char *const password = cliCommand.args[2];

	char buffer[USER_CSV_ENTRY_MAX_LENGTH + 1] = {[0] = '\0'};
	while (fgets(buffer, USER_CSV_ENTRY_MAX_LENGTH, users) != NULL) {
		// TODO: Use Vector API
		const char *const user = strtok(trim(buffer), CSV_DELIMITER);
		const char *const pass = strtok(NULL, CSV_DELIMITER);
		const char *const role = strtok(NULL, CSV_DELIMITER);

		if (strcmp(role, TEACHER_ROLE) != 0
		    && strcmp(role, STUDENT_ROLE) != 0) {
			continue;
		}

		if (strcmp(user, username) != 0) {
			continue;
		}

		if (strcmp(pass, password) != 0) {
			continue;
		}

		addClientLogin(loggedClients,
		               CLIENT_LOGGED_MAX,
		               globalCurrentReqIP,
		               strncmp(role, TEACHER_ROLE, 10) == 0 ? TEACHER
		                                                    : STUDENT);
		strcpy(response, LOGIN_SUCESS_CLIENT);
		break;
	}

	if (response[0] == '\0') {
		strcpy(response, INVALID_USER_OR_PASSWORD_CLIENT);
	}

	fclose(users);
}

void classListClassesCommand(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	response[0] = '\0';
	int shmFd   = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE);

	Class *classes_ptr
	    = (Class *) mmap(NULL,
	                     sizeof(struct Class) * CLASS_MAX_SIZE,
	                     PROT_READ | PROT_WRITE,
	                     MAP_SHARED,
	                     shmFd,
	                     0);

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		// if (classes_ptr[i].name != NULL) {
		// 	break;
		// }

		char ip[INET_ADDRSTRLEN] = {0};

		inet_ntop(AF_INET,
		          &classes_ptr[i].ipMulticast.sin_addr,
		          ip,
		          INET_ADDRSTRLEN);

		// printf("Class: %s, Ip: %s\n", classes_ptr[i].name, ip);

		printf("Created class %s with ip %s\n",
		       classes_ptr[i].name,
		       inet_ntoa(classes_ptr[i].ipMulticast.sin_addr));
	}

	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE);
	close(shmFd);

	sprintf(response, "Not implemented %s\n", usersFilepath);
}

void classListSubscribedCommand(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	response[0] = '\0';
	sprintf(response, "Not implemented %s\n", usersFilepath);
}

void classSubscribeClassCommand(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	response[0] = '\0';
	sprintf(response, "Not implemented %s\n", usersFilepath);
}

void classCreateClassCommand(const CliCommand cliCommand, char *response)
{
	response[0] = '\0';

	if (currentClient.role != TEACHER) {
		sprintf(response,
		        "You don't have permission to create a class.\n");
		return;
	}

	char *className       = cliCommand.args[1];
	const int maxStudents = atoi(cliCommand.args[2]);

	char *teste = malloc(strlen(className) + 1);
	teste[0]    = '\0';

	sprintf(teste, "%s", className);

	printf("className: %s\n", className);
	printf("Teste: %s\n", teste);


	int shmFd
	    = shm_open(CLASSES_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE);
	Class *classes_ptr
	    = (Class *) mmap(NULL,
	                     sizeof(struct Class) * CLASS_MAX_SIZE,
	                     PROT_READ | PROT_WRITE,
	                     MAP_SHARED,
	                     shmFd,
	                     0);

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		if (classes_ptr[i].maxStudents == 0) {
			classes_ptr[i] = (struct Class){
			    .name            = teste,
			    .maxStudents     = maxStudents,
			    .students        = {{0}},
			    .currentStudents = 0,
			    .ipMulticast     = createNewIpMultiCast(),
			};

			printf("Created class %s with ip %s\n",
			       classes_ptr[i].name,
			       inet_ntoa(classes_ptr[i].ipMulticast.sin_addr));

			break;
		}
	}

	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE);
	close(shmFd);

	sprintf(response, "OK ipTeste\n");
}

void classSendCommand(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	response[0] = '\0';
	sprintf(response, "Not implemented %s\n", usersFilepath);
}

void classHelpCommand(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	response[0] = '\0';
	sprintf(response, "Not implemented %s\n", usersFilepath);
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

	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE);
	Class *classes_ptr
	    = (Class *) mmap(NULL,
	                     sizeof(struct Class) * CLASS_MAX_SIZE,
	                     PROT_READ | PROT_WRITE,
	                     MAP_SHARED,
	                     shmFd,
	                     0);

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		classes_ptr[i] = (struct Class){
		    .name            = "",
		    .maxStudents     = 0,
		    .students        = {{0}},
		    .currentStudents = 0,
		    .ipMulticast     = {0},
		};
	}

	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE);
	close(shmFd);
}

void addClientLogin(struct LoggedClient *loggedClients,
                    const size_t loggedClientsSize,
                    const struct sockaddr_in clientIP,
                    const Role role)
{
	for (size_t i = 0; i < loggedClientsSize; ++i) {
		const struct LoggedClient c = loggedClients[i];

		if (c.clientAddrs.sin_addr.s_addr != 0) {
			continue;
		}

		currentClient = (struct LoggedClient){
		    .role        = role,
		    .clientAddrs = clientIP,
		};

		loggedClients[i] = currentClient;
		break;
	}
}
