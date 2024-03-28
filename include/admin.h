#ifndef ADMIN_H
#define ADMIN_H

#include "command.h"
#include "udp-server.h"

#define ADMIN_COMMAND_ENUM                                                     \
	WRAPPER(HELP, "help", "help", commandHelp)                             \
	WRAPPER(                                                               \
	    ADD_USER,                                                          \
	    "ADD_USER",                                                        \
	    "ADD_USER <username> <password> <administrador/aluno/professor> ", \
	    addUser)                                                           \
	WRAPPER(DELETE_USER,                                                   \
	        "DELETE_USER",                                                 \
	        "DELETE_USER <username>",                                      \
	        deleteUser)                                                    \
	WRAPPER(LIST_USERS, "LIST_USERS", "LIST_USERS", listUsers)             \
	WRAPPER(QUIT, "QUIT", "QUIT", quitServer)                              \
	WRAPPER(LOGIN, "LOGIN", "LOGIN <username> <password>", loginAdmin)

typedef enum {
#define WRAPPER(enum, text, usage, function) enum,
	ADMIN_COMMAND_ENUM
#undef WRAPPER
} AdminCommand;

void setupAdminConsole(UDPSocket *udpSocket, const char *const configFilepath);

AdminCommand processAdminCommand(const CliCommand cliCommand,
                                 char *responseBuffer,
                                 const size_t responseBufferSize);

#define LOGIN_SUCESS "Logged in succesfully!\n"

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"

void addUser(const CliCommand cliCommand, char *response);
void deleteUser(const CliCommand cliCommand, char *response);
void listUsers(const CliCommand cliCommand, char *response);
void quitServer(const CliCommand cliCommand, char *response);
void commandHelp(const CliCommand cliCommand, char *response);
void loginAdmin(const CliCommand cliCommand, char *response);

#endif // !ADMIN_H
