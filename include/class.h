#ifndef CLASS_H
#define CLASS_H

#include "command.h"
#include "tcp/socket.h"

#include <stdbool.h>

#define MOTD                            "Bem-vindo à turma\n"
#define LOGIN_SUCESS_CLIENT             "OK\n"
#define INVALID_USER_OR_PASSWORD_CLIENT "REJECTED\n"

#define TEACHER_ROLE     "professor"
#define STUDENT_ROLE     "aluno"
#define CLASSES_OBJ_NAME "/classes"

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"
#define CLIENT_LOGGED_MAX         10
#define CLASS_MAX_SIZE            10
#define MAX_CLASSES               10

#define ROLE_ENUM           \
	WRAPPER(GUEST, 0)   \
	WRAPPER(STUDENT, 1) \
	WRAPPER(TEACHER, 2)

typedef enum {
#define WRAPPER(ENUM, BIT_SHIFT) ENUM = 1 << BIT_SHIFT,
	ROLE_ENUM
#undef WRAPPER

	    ALL
	= 0
#define WRAPPER(ENUM, BIT_SHIFT) | ENUM
	ROLE_ENUM
#undef WRAPPER
	,
} Role;

#define CLASS_NOT_LOGGED_IN_COMMANDS                          \
	WRAPPER(CLASS_HELP, "help", "help", classHelpCommand) \
	WRAPPER(CLASS_LOGIN,                                  \
	        "LOGIN",                                      \
	        "LOGIN <username> <password>",                \
	        classLoginCommand)
#define CLASS_LOGGED_IN_COMMANDS              \
	WRAPPER(CLASS_LIST_CLASSES,           \
	        "LIST_CLASSES",               \
	        "LIST_CLASSES",               \
	        classListClassesCommand)      \
	WRAPPER(CLASS_LIST_SUBSCRIBED,        \
	        "LIST_SUBSCRIBED",            \
	        "LIST_SUBSCRIBED",            \
	        classListSubscribedCommand)   \
	WRAPPER(CLASS_SUBSCRIBE_CLASS,        \
	        "SUBSCRIBE_CLASS",            \
	        "SUBSCRIBE_CLASS <class>",    \
	        classSubscribeClassCommand)   \
	WRAPPER(CLASS_CREATE_CLASS,           \
	        "CREATE_CLASS",               \
	        "CREATE_CLASS <name> <size>", \
	        classCreateClassCommand)      \
	WRAPPER(CLASS_SEND, "SEND", "SEND <class> <text>", classSendCommand)
#define CLASS_COMMANDS               \
	CLASS_NOT_LOGGED_IN_COMMANDS \
	CLASS_LOGGED_IN_COMMANDS

typedef enum {
#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) ENUM,
	CLASS_COMMANDS
#undef WRAPPER
} ClassCommand;

#define WRAPPER(ENUM, COMMAND, USAGE, FUNCTION) \
	void FUNCTION(const CliCommand cliCommand, char *response);
CLASS_COMMANDS
#undef WRAPPER

typedef struct LoggedClient {
	struct sockaddr_in clientAddrs;
	Role role;
} LoggedClient;

typedef struct Class {
	char name[100];
	char ipMulticast[100];
	size_t maxStudents;
	size_t currentStudents;
	struct sockaddr_in students[CLASS_MAX_SIZE];
} Class;


ClassCommand processClassCommand(const CliCommand cliCommand,
                                 const bool isLoggedIn,
                                 char *responseBuffer,
                                 const size_t responseBufferSize);
bool isClientLogged(const struct sockaddr_in clientIP);
void addClientLogin(struct LoggedClient *loggedClients,
                    const size_t loggedClientsSize,
                    const struct sockaddr_in clientIP,
                    const Role role);
void setupClass(TCPSocket *tcpSocket);
void processClient(const TCPSocket clientTCPSocket);

#endif // !CLASS_H
