#include "command/class.h"
#include "tcp/socket.h"

#include <assert.h>

extern char *usersFilepath;

void classHelpPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	(void) command;
	clearTCPSocketBuffer(socket);
	sprintfToTCPSocketBuffer(socket, "Not implemented %s\n", usersFilepath);
}

void classLoginPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void classListClassesPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void classListSubscribedPreSendHook(const ClassCommand command,
                                    TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void classSubscribePreSendHook(const ClassCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void classCreateClassPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
}

void classSendPreSendHook(const ClassCommand command, TCPSocket *socket)
{
	assert(0 && "TODO: Not yet implemented\n");
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

/* void classLoginCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	response[0] = '\0'; */

/* 	if (usersFilepath == NULL) { */
/* 		return; */
/* 	} */

/* 	if (cliCommand.nargs != 3) { */
/* 		// TODO: usage */
/* 		return; */
/* 	} */

/* 	FILE *users = fopen(usersFilepath, "r"); */
/* 	if (users == NULL) { */
/* 		debugMessage(stderr, ERROR, "Opening file %s\n", usersFilepath); */
/* 		// TODO: I don't know if I agree that the response should be */
/* 		// this when failing to opening a file */
/* 		sprintf(response, "Error opening file %s\n", usersFilepath); */
/* 		return; */
/* 	} */

/* 	const char *const username = cliCommand.args[1]; */
/* 	const char *const password = cliCommand.args[2]; */

/* 	char buffer[USER_CSV_ENTRY_MAX_LENGTH + 1] = {[0] = '\0'}; */
/* 	while (fgets(buffer, USER_CSV_ENTRY_MAX_LENGTH, users) != NULL) { */
/* 		// TODO: Use Vector API */
/* 		const char *const user = strtok(trim(buffer), CSV_DELIMITER); */
/* 		const char *const pass = strtok(NULL, CSV_DELIMITER); */
/* 		const char *const role = strtok(NULL, CSV_DELIMITER); */

/* 		if (strcmp(role, TEACHER_ROLE) != 0 */
/* 		    && strcmp(role, STUDENT_ROLE) != 0) { */
/* 			continue; */
/* 		} */

/* 		if (strcmp(user, username) != 0) { */
/* 			continue; */
/* 		} */

/* 		if (strcmp(pass, password) != 0) { */
/* 			continue; */
/* 		} */

/* 		addClientLogin(loggedClients, */
/* 		               CLIENT_LOGGED_MAX, */
/* 		               globalCurrentReqIP, */
/* 		               strncmp(role, TEACHER_ROLE, 10) == 0 ? TEACHER */
/* 		                                                    : STUDENT); */
/* 		strcpy(response, LOGIN_SUCESS_CLIENT); */
/* 		break; */
/* 	} */

/* 	if (response[0] == '\0') { */
/* 		strcpy(response, INVALID_USER_OR_PASSWORD_CLIENT); */
/* 	} */

/* 	fclose(users); */
/* } */

/* void classListClassesCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	(void) cliCommand; */
/* 	response[0] = '\0'; */
/* 	int shmFd   = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR); */
/* 	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE); */

/* 	Class *classes_ptr */
/* 	    = (Class *) mmap(NULL, */
/* 	                     sizeof(struct Class) * CLASS_MAX_SIZE, */
/* 	                     PROT_READ | PROT_WRITE, */
/* 	                     MAP_SHARED, */
/* 	                     shmFd, */
/* 	                     0); */

/* 	sprintf(response, "Class "); */

/* 	bool isFirstClass = true; */

/* 	for (int i = 0; i < CLASS_MAX_SIZE; i++) { */
/* 		char ip[INET_ADDRSTRLEN] = {0}; */

/* 		inet_ntop(AF_INET, */
/* 		          &classes_ptr[i].ipMulticast.sin_addr, */
/* 		          ip, */
/* 		          INET_ADDRSTRLEN); */

/* 		if (classes_ptr[i].name[0] != '\0') { */
/* 			if (!isFirstClass) { */
/* 				strcat(response, ", "); */
/* 			} */
/* 			isFirstClass = false; */
/* 			strcat(response, classes_ptr[i].name); */
/* 		} */
/* 	} */

/* 	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	close(shmFd); */
/* } */

/* void classListSubscribedCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	(void) cliCommand; */
/* 	response[0] = '\0'; */

/* 	if (currentClient.role != STUDENT) { */
/* 		sprintf(response, "UNAUTHORIZED"); */
/* 		return; */
/* 	} */


/* 	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR); */
/* 	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE); */

/* 	Class *classes_ptr */
/* 	    = (Class *) mmap(NULL, */
/* 	                     sizeof(struct Class) * CLASS_MAX_SIZE, */
/* 	                     PROT_READ | PROT_WRITE, */
/* 	                     MAP_SHARED, */
/* 	                     shmFd, */
/* 	                     0); */


/* 	bool isFirstClass = true; */

/* 	sprintf(response, "Class "); */

/* 	for (int i = 0; i < CLASS_MAX_SIZE; i++) { */
/* 		if (classes_ptr[i].currentStudents == 0) { */
/* 			continue; */
/* 		} */

/* 		for (int j = 0; j < CLASS_MAX_SIZE; j++) { */
/* 			if (isSameAddress(classes_ptr[i].students[j], */
/* 			                  globalCurrentReqIP)) { */
/* 				char multicastIp[21] = {0}; */
/* 				sprintf( */
/* 				    multicastIp, */
/* 				    "%s:%d", */
/* 				    inet_ntoa( */
/* 				        classes_ptr[i].ipMulticast.sin_addr), */
/* 				    classes_ptr[i].ipMulticast.sin_port); */

/* 				if (!isFirstClass) { */
/* 					strcat(response, ", "); */
/* 				} */
/* 				isFirstClass = false; */

/* 				strcat(response, classes_ptr[i].name); */
/* 				strcat(response, "/"); */
/* 				strcat(response, multicastIp); */
/* 			} */
/* 		} */
/* 	} */
/* 	strcat(response, "\n"); */

/* 	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	close(shmFd); */
/* } */

/* void classSubscribeClassCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	(void) cliCommand; */
/* 	response[0] = '\0'; */

/* 	if (currentClient.role != STUDENT) { */
/* 		sprintf(response, "UNAUTHORIZED"); */
/* 		return; */
/* 	} */

/* 	char *className = cliCommand.args[1]; */

/* 	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR); */
/* 	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE); */

/* 	Class *classes_ptr */
/* 	    = (Class *) mmap(NULL, */
/* 	                     sizeof(struct Class) * CLASS_MAX_SIZE, */
/* 	                     PROT_READ | PROT_WRITE, */
/* 	                     MAP_SHARED, */
/* 	                     shmFd, */
/* 	                     0); */

/* 	sprintf(response, "REJECTED\n"); */

/* 	for (int i = 0; i < CLASS_MAX_SIZE; i++) { */
/* 		if (strcmp(classes_ptr[i].name, className) == 0) { */
/* 			if (classes_ptr[i].currentStudents */
/* 			    < classes_ptr[i].maxStudents) { */
/* 				classes_ptr[i] */
/* 				    .students[classes_ptr[i].currentStudents] */
/* 				    = globalCurrentReqIP; */
/* 				classes_ptr[i].currentStudents */
/* 				    = classes_ptr[i].currentStudents + 1; */
/* 				sprintf( */
/* 				    response, */
/* 				    "ACCEPTED %s\n", */
/* 				    inet_ntoa( */
/* 				        classes_ptr[i].ipMulticast.sin_addr)); */
/* 			} */
/* 			break; */
/* 		} */
/* 	} */

/* 	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	close(shmFd); */
/* } */

/* void classCreateClassCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	response[0] = '\0'; */

/* 	if (currentClient.role != TEACHER) { */
/* 		sprintf(response, "UNAUTHORIZED"); */
/* 		return; */
/* 	} */

/* 	char *className       = cliCommand.args[1]; */
/* 	const int maxStudents = atoi(cliCommand.args[2]); */

/* 	int shmFd */
/* 	    = shm_open(CLASSES_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); */

/* 	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	Class *classes_ptr */
/* 	    = (Class *) mmap(NULL, */
/* 	                     sizeof(struct Class) * CLASS_MAX_SIZE, */
/* 	                     PROT_READ | PROT_WRITE, */
/* 	                     MAP_SHARED, */
/* 	                     shmFd, */
/* 	                     0); */

/* 	for (int i = 0; i < CLASS_MAX_SIZE; i++) { */
/* 		if (classes_ptr[i].maxStudents == 0) { */
/* 			classes_ptr[i] = (struct Class){ */
/* 			    .name            = {0}, */
/* 			    .maxStudents     = maxStudents, */
/* 			    .students        = {{0}}, */
/* 			    .currentStudents = 0, */
/* 			    .ipMulticast     = createNewIpMultiCast(), */
/* 			}; */

/* 			strcpy(classes_ptr[i].name, className); */

/* 			printf("Created class %s with ip %s\n", */
/* 			       classes_ptr[i].name, */
/* 			       inet_ntoa(classes_ptr[i].ipMulticast.sin_addr)); */

/* 			sprintf(response, */
/* 			        "OK %s:%d\n", */
/* 			        inet_ntoa(classes_ptr[i].ipMulticast.sin_addr), */
/* 			        classes_ptr[i].ipMulticast.sin_port); */

/* 			break; */
/* 		} */
/* 	} */

/* 	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	close(shmFd); */
/* } */

/* void classSendCommand(const CliCommand cliCommand, char *response) */
/* { */
/* 	(void) cliCommand; */
/* 	response[0] = '\0'; */

/* 	if (currentClient.role != TEACHER) { */
/* 		sprintf(response, "UNAUTHORIZED"); */
/* 		return; */
/* 	} */

/* 	char *className = cliCommand.args[1]; */
/* 	char *msg       = cliCommand.args[2]; */

/* 	int shmFd = shm_open(CLASSES_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR); */
/* 	ftruncate(shmFd, sizeof(struct Class) * CLASS_MAX_SIZE); */

/* 	Class *classes_ptr */
/* 	    = (Class *) mmap(NULL, */
/* 	                     sizeof(struct Class) * CLASS_MAX_SIZE, */
/* 	                     PROT_READ | PROT_WRITE, */
/* 	                     MAP_SHARED, */
/* 	                     shmFd, */
/* 	                     0); */

/* 	sprintf(response, "REJECTED\n"); */

/* 	for (int i = 0; i < CLASS_MAX_SIZE; i++) { */
/* 		if (strcmp(classes_ptr[i].name, className) == 0) { */
/* 			int socketFD; */

/* 			if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { */
/* 				perror("socket creation failed"); */
/* 				sprintf(response, "ERROR\n"); */
/* 				continue; */
/* 			} */

/* 			// enable multicast on the socket */
/* 			int enable = 4; */
/* 			if (setsockopt(socketFD, */
/* 			               IPPROTO_IP, */
/* 			               IP_MULTICAST_TTL, */
/* 			               &enable, */
/* 			               sizeof(enable)) */
/* 			    < 0) { */
/* 				perror("setsockopt"); */
/* 				sprintf(response, "ERROR\n"); */
/* 				continue; */
/* 			} */

/* 			// send the multicast message */
/* 			if (sendto( */
/* 			        socketFD, */
/* 			        msg, */
/* 			        strnlen(msg, BUFFER_SIZE), */
/* 			        0, */
/* 			        (struct sockaddr *) &classes_ptr[i].ipMulticast, */
/* 			        sizeof(classes_ptr[i].ipMulticast)) */
/* 			    < 0) { */
/* 				perror("sendto"); */
/* 				sprintf(response, "ERROR\n"); */
/* 			} */

/* 			sprintf(response, "ACCEPTED\n"); */
/* 			break; */
/* 			close(socketFD); */
/* 		} */
/* 	} */

/* 	munmap(classes_ptr, sizeof(struct Class) * CLASS_MAX_SIZE); */
/* 	close(shmFd); */
/* } */
