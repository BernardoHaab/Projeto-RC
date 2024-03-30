#ifndef ADMIN_H
#define ADMIN_H

#include "command.h"
#include "udp/socket.h"

#include <stdbool.h>

#define ADMIN_COMMANDS                                                         \
	WRAPPER(ADMIN_HELP, "help", "help", commandHelp)                       \
	WRAPPER(                                                               \
	    ADMIN_ADD_USER,                                                    \
	    "ADD_USER",                                                        \
	    "ADD_USER <username> <password> <administrador/aluno/professor> ", \
	    addUser)                                                           \
	WRAPPER(ADMIN_DELETE_USER,                                             \
	        "DELETE_USER",                                                 \
	        "DELETE_USER <username>",                                      \
	        deleteUser)                                                    \
	WRAPPER(ADMIN_LIST_USERS, "LIST_USERS", "LIST_USERS", listUsers)       \
	WRAPPER(ADMIN_QUIT, "QUIT", "QUIT", quitServer)                        \
	WRAPPER(ADMIN_LOGIN, "LOGIN", "LOGIN <username> <password>", loginAdmin)

typedef enum {
#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) ENUM,
	ADMIN_COMMANDS
#undef WRAPPER
} AdminCommand;

void setupAdminConsole(UDPSocket *serverUDPSocket);

AdminCommand processAdminCommand(const CliCommand cliCommand,
                                 char *responseBuffer,
                                 const size_t responseBufferSize);

#define LOGIN_SUCESS         "Logged in succesfully!\n"
#define USER_NOT_FOUND       "User not found!\n"
#define USER_DELETED_SUCCESS "User was deleted with success!\n"
#define ADMIN_LOGGED_MAX     10

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"

bool isLogged(const struct sockaddr_in clientIP);

#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) \
	void FUNCTION(const CliCommand cliCommand, char *response);
ADMIN_COMMANDS
#undef WRAPPER

#endif // !ADMIN_H
