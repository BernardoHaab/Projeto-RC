#include "admin.h"
#include "class.h"
#include "command.h"
#include "command/class.h"
#include "debug.h"
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
#include <unistd.h>

extern Vector loggedClients;
extern char *usersFilepath;

void classHelpPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	(void) command;
	clearTCPSocketBuffer(socket);
	sprintfToTCPSocketBuffer(socket, "Not implemented %s", usersFilepath);
}

void classLoginPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	clearTCPSocketBuffer(socket);

	if (usersFilepath == NULL) {
		debugMessage(stdout, ERROR, "Users filepath not set");
		return;
	}

	if (command.args.size != 3) {
		debugMessage(stderr, ERROR, "Invalid number of arguments");
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		sprintfToTCPSocketBuffer(socket,
		                         "Comando inválido. Tente: %s",
		                         command.usageArgs);
		return;
	}

	FILE *users = fopen(usersFilepath, "r");
	if (users == NULL) {
		debugMessage(stderr, ERROR, "Opening file %s", usersFilepath);
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		sprintfToTCPSocketBuffer(socket,
		                         "Error opening file %s",
		                         usersFilepath);
		return;
	}

	const char *const username = *(char **) vectorGet(&command.args, 1);
	const char *const password = *(char **) vectorGet(&command.args, 2);

	char buffer[USER_CSV_ENTRY_MAX_LENGTH + 1] = {[0] = '\0'};
	bool found                                 = false;
	while (!found
	       && fgets(buffer, USER_CSV_ENTRY_MAX_LENGTH, users) != NULL) {
		// TODO: Use Vector API
		const char *const user = strtok(trim(buffer), CSV_DELIMITER);
		const char *const pass = strtok(NULL, CSV_DELIMITER);
		const char *const role = strtok(NULL, CSV_DELIMITER);

		if (strcmp(role, ADMIN_ROLE) == 0) {
			continue;
		}

		if (strcmp(user, username) != 0) {
			continue;
		}

		if (strcmp(pass, password) != 0) {
			continue;
		}

		addClientLogin(&loggedClients,
		               socket->address,
		               parseRole(role));

		found = true;
	}

	writeToTCPSocketBuffer(socket,
	                       found ? LOGIN_SUCESS : INVALID_USER_OR_PASSWORD);

	fclose(users);
}

void classListClassesPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	(void) command;
	clearTCPSocketBuffer(socket);

	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmFd, sizeof(Class) * CLASS_MAX_SIZE);

	Class *classes_ptr = (Class *) mmap(NULL,
	                                    sizeof(Class) * CLASS_MAX_SIZE,
	                                    PROT_READ | PROT_WRITE,
	                                    MAP_SHARED,
	                                    shmFd,
	                                    0);

	char response[4096] = {0};
	sprintf(response, "Class ");

	bool isFirstClass = true;

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		char ip[INET_ADDRSTRLEN] = {0};

		inet_ntop(AF_INET,
		          &classes_ptr[i].ipMulticast.sin_addr,
		          ip,
		          INET_ADDRSTRLEN);

		if (classes_ptr[i].name[0] != '\0') {
			if (!isFirstClass) {
				strcat(response, ", ");
			}
			isFirstClass = false;
			strcat(response, classes_ptr[i].name);
		}
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);

	writeToTCPSocketBuffer(socket, response);
}

void classListSubscribedPreSendHook(const ClassCommand command,
                                    TCPSocket *socket)
{
	(void) command;

	clearTCPSocketBuffer(socket);

	if (getLoggedClientRole(loggedClients, socket->address)
	    != ROLE_STUDENT) {
		sprintfToTCPSocketBuffer(socket, "UNAUTHORIZED");
		return;
	}


	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmFd, sizeof(Class) * CLASS_MAX_SIZE);

	Class *classes_ptr = (Class *) mmap(NULL,
	                                    sizeof(Class) * CLASS_MAX_SIZE,
	                                    PROT_READ | PROT_WRITE,
	                                    MAP_SHARED,
	                                    shmFd,
	                                    0);


	bool isFirstClass = true;

	char response[4096] = {0};
	sprintf(response, "Class ");

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		if (classes_ptr[i].currentStudents == 0) {
			continue;
		}

		for (int j = 0; j < CLASS_MAX_SIZE; j++) {
			if (!isSameAddress(classes_ptr[i].students[j],
			                   socket->address)) {
				continue;
			}

			char multicastIp[INET_ADDRSTRLEN + 6] = {0};
			snprintf(multicastIp,
			         INET_ADDRSTRLEN + 5,
			         "%s:%d",
			         inet_ntoa(classes_ptr[i].ipMulticast.sin_addr),
			         classes_ptr[i].ipMulticast.sin_port);

			if (!isFirstClass) {
				strcat(response, ", ");
			}

			isFirstClass = false;

			strcat(response, classes_ptr[i].name);
			strcat(response, "/");
			strcat(response, multicastIp);
		}
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);

	writeToTCPSocketBuffer(socket, response);
}

void classSubscribePreSendHook(const ClassCommand command, TCPSocket *socket)
{
	(void) command;

	clearTCPSocketBuffer(socket);

	if (getLoggedClientRole(loggedClients, socket->address)
	    != ROLE_STUDENT) {
		sprintfToTCPSocketBuffer(socket, "UNAUTHORIZED");
		return;
	}

	const char *const className = *(char **) vectorGet(&command.args, 1);

	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmFd, sizeof(Class) * CLASS_MAX_SIZE);

	Class *classes_ptr = (Class *) mmap(NULL,
	                                    sizeof(Class) * CLASS_MAX_SIZE,
	                                    PROT_READ | PROT_WRITE,
	                                    MAP_SHARED,
	                                    shmFd,
	                                    0);

	sprintfToTCPSocketBuffer(socket, COMMAND_REPLY_REJECTED);

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		if (strcmp(classes_ptr[i].name, className) != 0
		    || classes_ptr[i].currentStudents
		           >= classes_ptr[i].maxStudents) {
			continue;
		}

		classes_ptr[i].students[classes_ptr[i].currentStudents++]
		    = socket->address;
		sprintfToTCPSocketBuffer(
		    socket,
		    COMMAND_REPLY_ACCEPTED " %s",
		    inet_ntoa(classes_ptr[i].ipMulticast.sin_addr));
		break;
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);
}

void classCreateClassPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	clearTCPSocketBuffer(socket);

	if (getLoggedClientRole(loggedClients, socket->address)
	    != ROLE_TEACHER) {
		sprintfToTCPSocketBuffer(socket, "UNAUTHORIZED");
		return;
	}

	const char *const className = *(char **) vectorGet(&command.args, 1);
	const int maxStudents = atoi(*(char **) vectorGet(&command.args, 2));

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
		if (classes_ptr[i].maxStudents != 0) {
			continue;
		}

		classes_ptr[i] = (Class){
		    .name            = {0},
		    .maxStudents     = maxStudents,
		    .students        = {{0}},
		    .currentStudents = 0,
		    .ipMulticast     = createNewIpMultiCast(),
		};

		strcpy(classes_ptr[i].name, className);

		debugMessage(stdout,
		             OK,
		             "Created class %s with ip %s",
		             classes_ptr[i].name,
		             inet_ntoa(classes_ptr[i].ipMulticast.sin_addr));

		sprintfToTCPSocketBuffer(
		    socket,
		    COMMAND_REPLY_OK " %s:%d",
		    inet_ntoa(classes_ptr[i].ipMulticast.sin_addr),
		    classes_ptr[i].ipMulticast.sin_port);

		break;
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);
}

void classSendPreSendHook(const ClassCommand command, TCPSocket *tcpSocket)
{
	clearTCPSocketBuffer(tcpSocket);

	if (getLoggedClientRole(loggedClients, tcpSocket->address)
	    != ROLE_TEACHER) {
		sprintfToTCPSocketBuffer(tcpSocket, "UNAUTHORIZED");
		return;
	}

	const char *const className = *(char **) vectorGet(&command.args, 1);
	// TODO: Add remaning args to message
	const char *const msg       = *(char **) vectorGet(&command.args, 2);

	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmFd, sizeof(Class) * CLASS_MAX_SIZE);

	Class *classes_ptr = (Class *) mmap(NULL,
	                                    sizeof(Class) * CLASS_MAX_SIZE,
	                                    PROT_READ | PROT_WRITE,
	                                    MAP_SHARED,
	                                    shmFd,
	                                    0);

	sprintfToTCPSocketBuffer(tcpSocket, COMMAND_REPLY_REJECTED "");

	for (int i = 0; i < CLASS_MAX_SIZE; i++) {
		if (strcmp(classes_ptr[i].name, className) == 0) {
			int socketFD;

			if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
				perror("socket creation failed");
				sprintfToTCPSocketBuffer(tcpSocket, "ERROR");
				continue;
			}

			// enable multicast on the socket
			int enable = 4;
			if (setsockopt(socketFD,
			               IPPROTO_IP,
			               IP_MULTICAST_TTL,
			               &enable,
			               sizeof(enable))
			    < 0) {
				perror("setsockopt");
				sprintfToTCPSocketBuffer(tcpSocket, "ERROR");
				continue;
			}

			// send the multicast message
			if (sendto(
			        socketFD,
			        msg,
			        strnlen(msg, BUFFER_SIZE),
			        0,
			        (struct sockaddr *) &classes_ptr[i].ipMulticast,
			        sizeof(classes_ptr[i].ipMulticast))
			    < 0) {
				perror("sendto");
				sprintfToTCPSocketBuffer(tcpSocket, "ERROR");
			}

			sprintfToTCPSocketBuffer(tcpSocket,
			                         COMMAND_REPLY_ACCEPTED);
			close(socketFD);
			break;
		}
	}

	munmap(classes_ptr, sizeof(Class) * CLASS_MAX_SIZE);
	close(shmFd);
}

/* ClassCommand processClassCommand(const CliCommand cliCommand, */
/*                                  const bool isLoggedIn, */
/*                                  char *responseBuffer, */
/*                                  const size_t responseBufferSize) */
/* { */
/* 	if (cliCommand.nargs <= 0) { */
/* 		strncpy(responseBuffer, */
/* 		        "Command not found. Tip: Type \"help\"\n", */
/* 		        responseBufferSize); */
/* 		return CLASS_HELP; */
/* 	} */

/* 	bool commandFound    = false; */
/* 	ClassCommand command = CLASS_HELP; */
/* 	do { */
/* #define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION)         \ */
/* 	if (strcmp(cliCommand.args[0], COMMAND) == 0) { \ */
/* 		commandFound = true;                    \ */
/* 		command      = ENUM;                    \ */
/* 		FUNCTION(cliCommand, responseBuffer);   \ */
/* 		break;                                  \ */
/* 	} */
/* 		CLASS_NOT_LOGGED_IN_COMMANDS */
/* #undef WRAPPER */
/* #define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION)               \ */
/* 	if (strcmp(cliCommand.args[0], COMMAND) == 0) {       \ */
/* 		commandFound = true;                          \ */
/* 		if (isLoggedIn) {                             \ */
/* 			command = ENUM;                       \ */
/* 			FUNCTION(cliCommand, responseBuffer); \ */
/* 		}                                             \ */
/* 		break;                                        \ */
/* 	} */
/* 		CLASS_LOGGED_IN_COMMANDS */
/* #undef WRAPPER */
/* 	} while (false); */

/* 	if (!commandFound) { */
/* 		strncpy(responseBuffer, */
/* 		        "Command not found. Tip: Type \"help\"\n", */
/* 		        responseBufferSize); */
/* 	} */

/* 	return command; */
/* } */
