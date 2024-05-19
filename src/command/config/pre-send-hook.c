#include "admin.h"
#include "command/config.h"
#include "command/config/hooks.h"
#include "debug.h"
#include "udp/socket.h"
#include "utils.h"
#include "vector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

extern Vector loggedAdmins;
extern char *usersFilepath;

void configHelpPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	(void) command;

	writeToUDPSocketBuffer(socket,
	                       "List of available commands:"
#define COMMAND(ENUM,              \
                COMMAND,           \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	"\n- " #COMMAND " " USAGE_ARGS
	                       CONFIG_COMMANDS
#undef COMMAND
	                       "\n");
	printf("%s", socket->buffer);
}

void configLoginPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	clearUDPSocketBuffer(socket);

	if (usersFilepath == NULL) {
		debugMessage(stderr, ERROR, "usersFilepath not set\n");
		return;
	}

	if (command.args.size != 3) {
		sprintfToUDPSocketBuffer(socket,
		                         "Comando inválido. Tente: %s\n",
		                         command.usageArgs);
		return;
	}

	FILE *users = fopen(usersFilepath, "r");
	if (users == NULL) {
		debugMessage(stderr, ERROR, "Opening file %s\n", usersFilepath);
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		sprintfToUDPSocketBuffer(socket,
		                         "Error opening file %s\n",
		                         usersFilepath);
		return;
	}

	const char *const username = *(char **) vectorGet(&command.args, 1);
	const char *const password = *(char **) vectorGet(&command.args, 2);

	char buffer[USER_CSV_ENTRY_MAX_LENGTH + 1] = {[0] = '\0'};
	bool validAdmin                            = false;
	while (fgets(buffer, USER_CSV_ENTRY_MAX_LENGTH, users) != NULL) {
		// TODO: Use Vector API
		const char *const user = strtok(trim(buffer), CSV_DELIMITER);
		const char *const pass = strtok(NULL, CSV_DELIMITER);
		const char *const role = strtok(NULL, CSV_DELIMITER);

		if (strcmp(role, ADMIN_ROLE) != 0) {
			continue;
		}

		if (strcmp(user, username) != 0) {
			continue;
		}

		if (strcmp(pass, password) != 0) {
			continue;
		}

		addLogin(&loggedAdmins, socket->address);
		validAdmin = true;
		break;
	}

	writeToUDPSocketBuffer(socket,
	                       validAdmin ? LOGIN_SUCESS "\n"
	                                  : INVALID_USER_OR_PASSWORD "\n");
	debugMessage(stdout, INFO, "%s", socket->buffer);

	fclose(users);
}

void configAddUserPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	clearUDPSocketBuffer(socket);

	if (!isLogged(&loggedAdmins, socket->address)) {
		debugMessage(stderr, ERROR, "Not logged in\n");
		sprintfToUDPSocketBuffer(socket, "Not logged in\n");
		return;
	}

	if (usersFilepath == NULL) {
		debugMessage(stdout, ERROR, "Users filepath not set\n");
		return;
	}

	// TODO: Move command integrity to postReceiveHook
	if (command.args.size != 4) {
		sprintfToUDPSocketBuffer(
		    socket,
		    "Comando inválido. Tente: %s %s\n",
		    configCommandTypeToString(command.command),
		    command.usageArgs);
		return;
	}

	const char *const login    = *(char **) vectorGet(&command.args, 1);
	const char *const password = *(char **) vectorGet(&command.args, 2);
	const char *const role     = *(char **) vectorGet(&command.args, 3);

	char newUserCSV[USER_CSV_ENTRY_MAX_LENGTH + 1] = {0};
	snprintf(newUserCSV,
	         USER_CSV_ENTRY_MAX_LENGTH,
	         "%s;%s;%s\n",
	         login,
	         password,
	         role);

	FILE *users = fopen(usersFilepath, "a");
	if (users == NULL) {
		debugMessage(stdout, ERROR, "Error opening users file\n");
		return;
	}

	fwrite(newUserCSV,
	       strnlen(newUserCSV, USER_CSV_ENTRY_MAX_LENGTH),
	       1,
	       users);

	fclose(users);

	debugMessage(stdout, OK, "Added user: %s\n", login);

	writeToUDPSocketBuffer(socket, "Usário adicionado com sucesso!\n");
}

void configDeleteUserPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	clearUDPSocketBuffer(socket);

	if (!isLogged(&loggedAdmins, socket->address)) {
		debugMessage(stderr, ERROR, "Not logged in\n");
		sprintfToUDPSocketBuffer(socket, "Not logged in\n");
		return;
	}

#define TEMP_FILENAME "temp.txt"

	if (usersFilepath == NULL) {
		debugMessage(stdout, ERROR, "Users filepath not set\n");
		return;
	}

	if (command.args.size != 2) {
		sprintfToUDPSocketBuffer(socket,
		                         "Comando inválido. Tente: %s\n",
		                         command.usageArgs);
		return;
	}

	const char *const userToDelete = *(char **) vectorGet(&command.args, 1);

	FILE *originalFile = fopen(usersFilepath, "r");
	if (originalFile == NULL) {
		debugMessage(stderr, ERROR, "Opening file %s\n", usersFilepath);
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		writeToUDPSocketBuffer(socket, USER_NOT_FOUND);
		return;
	}

	FILE *tempFile = fopen(TEMP_FILENAME, "w");
	if (tempFile == NULL) {
		debugMessage(stderr,
		             ERROR,
		             "Creating temporary file: %s\n",
		             TEMP_FILENAME);
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
#if DEBUG
		printf("Usuário:             %s\n", user);
		printf("Usuário para apagar: %s\n", userToDelete);
#endif

		if (!found && strcmp(userToDelete, user) == 0) {
			found = true;
			continue;
		}

		fputs(line, tempFile);
	}

	if (!found) {
		debugMessage(stderr,
		             ERROR,
		             "User not found: %s\n",
		             userToDelete);
	}

	fclose(originalFile);
	fclose(tempFile);

	remove(usersFilepath);
	rename(TEMP_FILENAME, usersFilepath);

	writeToUDPSocketBuffer(socket,
	                       found ? USER_DELETED_SUCCESS "\n"
	                             : USER_NOT_FOUND "\n");
}

void configListUsersPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	clearUDPSocketBuffer(socket);

	if (!isLogged(&loggedAdmins, socket->address)) {
		debugMessage(stderr, ERROR, "Not logged in\n");
		sprintfToUDPSocketBuffer(socket, "Not logged in\n");
		return;
	}

	if (usersFilepath == NULL) {
		debugMessage(stdout, ERROR, "Users filepath not set\n");
		return;
	}

	// BUG: Empty response.
	// STEPS TO REPRODUCE: Type the command LIST with additional arguments
	// and then just the LIST command without arguments
	if (command.args.size != 1) {
		sprintfToUDPSocketBuffer(socket,
		                         "Comando inválido. Tente: %s\n",
		                         command.usageArgs);
		return;
	}

	FILE *users = fopen(usersFilepath, "r");
	if (users == NULL) {
		debugMessage(stderr, ERROR, "Opening file %s\n", usersFilepath);
		// TODO: I don't know if I agree that the response should be
		// this when failing to opening a file
		sprintfToUDPSocketBuffer(socket,
		                         "Error opening file %s\n",
		                         usersFilepath);
		return;
	}

	char buffer[256];
	char userList[BUFFER_SIZE + 1] = {[0] = '\0', [BUFFER_SIZE] = '\0'};
	while (fgets(buffer, 256, users) != NULL) {
		char *user = strtok(buffer, CSV_DELIMITER);
		strcat(userList, user);
		strcat(userList, "\n");
	}

	writeToUDPSocketBuffer(socket, userList);

	fclose(users);
}

void configQuitServerPreSendHook(const ConfigCommand command, UDPSocket *socket)
{
	(void) command;

	if (!isLogged(&loggedAdmins, socket->address)) {
		debugMessage(stderr, ERROR, "Not logged in\n");
		sprintfToUDPSocketBuffer(socket, "Not logged in\n");
		return;
	}
	writeToUDPSocketBuffer(socket, "Closing server...\n");
}
