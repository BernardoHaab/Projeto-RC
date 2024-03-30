#ifndef ADMIN_H
#define ADMIN_H

#include "command.h"
#include "udp/socket.h"

#include <stdbool.h>

#define ADMIN_NOT_LOGGED_IN_COMMANDS                          \
	WRAPPER(ADMIN_HELP, "help", "help", adminHelpCommand) \
	WRAPPER(ADMIN_LOGIN,                                  \
	        "LOGIN",                                      \
	        "LOGIN <username> <password>",                \
	        adminLoginCommand)

#define ADMIN_LOGGED_IN_COMMANDS                                           \
	WRAPPER(ADMIN_ADD_USER,                                            \
	        "ADD_USER",                                                \
	        "ADD_USER <username> <password> <admin/student/teacher> ", \
	        adminAddUserCommand)                                       \
	WRAPPER(ADMIN_DELETE_USER,                                         \
	        "DEL",                                                     \
	        "DEL <username>",                                          \
	        adminDeleteUserCommand)                                    \
	WRAPPER(ADMIN_LIST_USERS,                                          \
	        "LIST_USERS",                                              \
	        "LIST_USERS",                                              \
	        adminListUsersCommand)                                     \
	WRAPPER(ADMIN_QUIT_SERVER,                                         \
	        "QUIT_SERVER",                                             \
	        "QUIT_SERVER",                                             \
	        adminQuitServerCommand)

#define ADMIN_COMMANDS               \
	ADMIN_NOT_LOGGED_IN_COMMANDS \
	ADMIN_LOGGED_IN_COMMANDS

typedef enum {
#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) ENUM,
	ADMIN_COMMANDS
#undef WRAPPER
} AdminCommand;

void setupAdminConsole(UDPSocket *serverUDPSocket);

AdminCommand processAdminCommand(const CliCommand cliCommand,
                                 char *responseBuffer,
                                 const size_t responseBufferSize);

#define LOGIN_SUCESS             "Logged in succesfully!\n"
#define INVALID_USER_OR_PASSWORD "Invalid User or Password\n"
#define USER_NOT_FOUND           "User not found!\n"
#define USER_DELETED_SUCCESS     "User was deleted with success!\n"
#define ADMIN_LOGGED_MAX         10

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"

bool isLogged(const struct sockaddr_in clientIP);

#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) \
	void FUNCTION(const CliCommand cliCommand, char *response);
ADMIN_COMMANDS
#undef WRAPPER

#endif // !ADMIN_H
