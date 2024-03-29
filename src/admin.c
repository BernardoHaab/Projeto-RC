#include "admin.h"

#include "command.h"
#include "debug.h"
#include "udp-server.h"

#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

char *usersFilepath = NULL;
struct sockaddr_in loggedAdmins[ADMIN_LOGGED_MAX];

void setupAdminConsole(UDPSocket *udpSocket, const char *const configFilepath)
{
	printf("Started Admin Console\n");

	usersFilepath = malloc(sizeof(usersFilepath[0]) * 256);
	usersFilepath = strcpy(usersFilepath, configFilepath);

	CliCommand cliCommand = {0};
	AdminCommand command  = ADMIN_HELP;

	while (command != ADMIN_QUIT) {
		const struct sockaddr_in clientIP
		    = receiveFromUDPSocket(udpSocket);

		char response[BUFFER_SIZE + 1]
		    = {[0] = '\0', [BUFFER_SIZE] = '\0'};

		parseCliCommand(udpSocket->buffer, &cliCommand);

		debugMessage(stdout,
		             INFO,
		             "Received: \"%s\" from ",
		             udpSocket->buffer);
		printSocketIP(stdout, true, clientIP);

		debugMessage(stdout, INFO, "");
		printCliCommand(stdout, cliCommand);

		command
		    = processAdminCommand(cliCommand, response, BUFFER_SIZE);

		if (sendto(udpSocket->fileDescriptor,
		           response,
		           strnlen(response, BUFFER_SIZE),
		           0,
		           (struct sockaddr *) &clientIP,
		           sizeof(clientIP))
		    < 0) {
			error("Error sendto");
		}
	}

	free(usersFilepath);
	destroyCliCommand(&cliCommand);

	// TODO: Corrigir o close do socket?
	close(udpSocket->fileDescriptor);
}

AdminCommand processAdminCommand(const CliCommand cliCommand,
                                 char *responseBuffer,
                                 const size_t responseBufferSize)
{
	if (cliCommand.nargs <= 0) {
		strncpy(responseBuffer,
		        "Command not found. Tip: Type \"help\"\n",
		        responseBufferSize);
		return ADMIN_HELP;
	}

	const size_t commandLength = strlen(cliCommand.args[0]);

	bool commandFound    = false;
	AdminCommand command = ADMIN_HELP;
#define WRAPPER(enum, text, usage, function)                         \
	if (strncmp(cliCommand.args[0], text, commandLength) == 0) { \
		commandFound = true;                                 \
		command      = enum;                                 \
		function(cliCommand, responseBuffer);                \
	}
	ADMIN_COMMAND_ENUM
#undef WRAPPER

	if (!commandFound) {
		strncpy(responseBuffer,
		        "Command not found. Tip: Type \"help\"\n",
		        responseBufferSize);
	}

	return command;
}

void addUser(const CliCommand cliCommand, char *response)
{
	if (usersFilepath == NULL) {
		return;
	}

	if (cliCommand.nargs != 4) {
		// TODO: Usage
		return;
	}

	const char *const login    = cliCommand.args[1];
	const char *const password = cliCommand.args[2];
	const char *const role     = cliCommand.args[3];

	char newUserCSV[USER_CSV_ENTRY_MAX_LENGTH + 1] = {0};
	snprintf(newUserCSV,
	         USER_CSV_ENTRY_MAX_LENGTH,
	         "%s;%s;%s\n",
	         login,
	         password,
	         role);

	FILE *users = fopen(usersFilepath, "a");
	if (users == NULL) {
		// TODO: Give error message opening users file
		return;
	}

	fwrite(newUserCSV,
	       strnlen(newUserCSV, USER_CSV_ENTRY_MAX_LENGTH),
	       1,
	       users);

	fclose(users);

	strcpy(response, "Usuário adicionado com sucesso!\n");
}

void deleteUser(const CliCommand cliCommand, char *response)
{
	response[0] = '\0';

#define TEMP_FILENAME "temp.txt"

	if (usersFilepath == NULL) {
		return;
	}

	if (cliCommand.nargs != 2) {
		// TODO: Usage
		return;
	}

	const char *const userToDelete = cliCommand.args[1];

	FILE *originalFile = fopen(usersFilepath, "r");
	if (originalFile == NULL) {
		fprintf(stderr, "Erro ao abrir arquivo de usuários\n");
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		strcpy(response, USER_NOT_FOUND);
		return;
	}

	FILE *tempFile = fopen(TEMP_FILENAME, "w");
	if (tempFile == NULL) {
		fprintf(stderr, "Error creating temporary file.\n");
		fclose(originalFile);
		return;
	}

	char *line    = NULL;
	size_t length = 0;
	bool found    = false;

	while (getline(&line, &length, originalFile) != -1) {
		char lineCopy[USER_CSV_ENTRY_MAX_LENGTH + 1] = {0};
		strncpy(lineCopy, line, USER_CSV_ENTRY_MAX_LENGTH);

		char *user = strtok(lineCopy, CSV_DELIMITER);
		printf("Usuário:             %s\n", user);
		printf("Usuário para apagar: %s\n", userToDelete);

		if (!found && strcmp(userToDelete, user) == 0) {
			found = true;
			continue;
		}

		fputs(line, tempFile);
	}

	if (!found) {
		fprintf(stderr, USER_NOT_FOUND);
		strcpy(response, USER_NOT_FOUND);
	}

	fclose(originalFile);
	fclose(tempFile);

	remove(usersFilepath);

	rename(TEMP_FILENAME, usersFilepath);

	strcpy(response, USER_DELETED_SUCCESS);
}

void listUsers(const CliCommand cliCommand, char *response)
{
	response[0] = '\0';

	if (usersFilepath == NULL) {
		return;
	}

	// BUG: Empty response.
	// STEPS TO REPRODUCE: Type the command LIST with additional arguments
	// and then just the LIST command without arguments
	if (cliCommand.nargs != 1) {
		// TODO: Usage
		return;
	}

	FILE *users = fopen(usersFilepath, "r");
	if (users == NULL) {
		fprintf(stderr, "Erro ao abrir arquivo de usuários\n");
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		strcpy(response, "Erro ao abrir arquivo de usuários\n");
		return;
	}

	char buffer[256];
	char userList[BUFFER_SIZE + 1] = {[0] = '\0', [BUFFER_SIZE] = '\0'};
	while (fgets(buffer, 256, users) != NULL) {
		char *user = strtok(buffer, CSV_DELIMITER);
		strcat(userList, user);
		strcat(userList, "\n");
	}

	strcpy(response, userList);

	fclose(users);
}

void quitServer(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;
	strcpy(response, "Closing server...!\n");
}

void commandHelp(const CliCommand cliCommand, char *response)
{
	(void) cliCommand;

	strcpy(response,
	       "List of available commands:"
#define WRAPPER(enum, text, usage, function) "\n- " usage
	       ADMIN_COMMAND_ENUM
#undef WRAPPER
	       "\n");
}

void loginAdmin(const CliCommand cliCommand, char *response)
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
		char *user = strtok(buffer, ";");
		char *pass = strtok(NULL, ";");
		char *role = strtok(NULL, "");
		printf("role: %s\n", role);

		if (strcmp(username, user) == 0 && strcmp(pass, password) == 0
		    && strcmp(role, "administrador\n") == 0) {
			strcpy(response, LOGIN_SUCESS);
			break;
		}
	}

	if (response[0] == '\0') {
		strcpy(response, "Invalid User or Password\n");
	}

	fclose(users);
}

bool isLogged(const struct sockaddr_in clientIP)
{
	for (int i = 0; i < ADMIN_LOGGED_MAX; i++) {
		bool isSameIp = loggedAdmins[i].sin_addr.s_addr
		                == clientIP.sin_addr.s_addr;
		bool isSamePort = loggedAdmins[i].sin_port == clientIP.sin_port;

		if (isSameIp && isSamePort) {
			return true;
		}
	}

	return false;
}
