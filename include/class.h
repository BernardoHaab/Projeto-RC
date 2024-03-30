#ifndef CLASS_H
#define CLASS_H

#include "command.h"
#include "tcp/socket.h"

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

void setupClass(TCPSocket *tcpSocket);

#endif // !CLASS_H
