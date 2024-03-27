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

void setupAdminConsole(UDPSocket *udpSocket, const char *const configFilepath)
{
	printf("Started Admin Console\n");

	usersFilepath = malloc(sizeof(usersFilepath[0]) * 256);
	usersFilepath = strcpy(usersFilepath, configFilepath);

	CliCommand cliCommand = {0};
	AdminCommand command  = HELP;

	while (command != QUIT) {
		const struct sockaddr_in clientIP
		    = receiveFromUDPSocket(udpSocket);

		char response[BUFFER_SIZE + 1]
		    = {[0] = '\0', [BUFFER_SIZE] = '\0'};
		parseCliCommand(udpSocket->buffer, &cliCommand);
		sprintCliCommand(response, cliCommand);

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
		return HELP;
	}

	const size_t commandLength = strlen(cliCommand.args[0]);

	bool commandFound    = false;
	AdminCommand command = HELP;
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
#define TEMP_FILENAME "temp.txt"

	if (usersFilepath == NULL || cliCommand.nargs != 2) {
		return;
	}

	FILE *originalFile = NULL;
	FILE *tempFile     = NULL;

	const char *const userToDelete = cliCommand.args[1];

	originalFile = fopen(usersFilepath, "r");
	if (originalFile == NULL) {
		fprintf(stderr, "Erro ao abrir arquivo de usuários\n");
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		strcpy(response, "Usuário não encontrado\n");
		return;
	}

	tempFile = fopen(TEMP_FILENAME, "w");
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
		fprintf(stderr, "Usuário não encontrado\n");
		strcpy(response, "Usuário não encontrado\n");
	}

	fclose(originalFile);
	fclose(tempFile);

	remove(usersFilepath);

	rename(TEMP_FILENAME, usersFilepath);
}

void listUsers(const CliCommand cliCommand, char *response)
{
	if (usersFilepath == NULL || cliCommand.nargs != 1) {
		return;
	}

	char buffer[256];
	FILE *users = fopen(usersFilepath, "r");
	char userList[BUFFER_SIZE];

	if (users == NULL) {
		fprintf(stderr, "Erro ao abrir arquivo de usuários\n");
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		strcpy(response, "Erro ao abrir arquivo de usuários\n");
		return;
	}

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
	strcpy(response, "Help...\n");
}
