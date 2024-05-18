#ifndef COMMAND_CLASS_H
#define COMMAND_CLASS_H

#include "command.h"
#include "tcp/socket.h"

#define CLASS_COMMANDS                         \
	COMMAND(COMMAND_HELP,                  \
	        ROLE_GUEST,                    \
	        help,                          \
	        "",                            \
	        -1,                            \
	        -1,                            \
	        HelpPreSendHook,               \
	        HelpPostReceiveHook)      \
	COMMAND(COMMAND_LOGIN,                 \
	        ROLE_GUEST,                    \
	        LOGIN,                         \
	        "<username> <password>",       \
	        2,                             \
	        2,                             \
	        LoginPreSendHook,              \
	        LoginPostReceiveHook)          \
	COMMAND(COMMAND_LIST_CLASSES,          \
	        ROLE_STUDENT,                  \
	        LIST_CLASSES,                  \
	        "",                            \
	        0,                             \
	        0,                             \
	        ListClassesPreSendHook,        \
	        ListClassesPostReceiveHook)    \
	COMMAND(COMMAND_LIST_SUBSCRIBED,       \
	        ROLE_STUDENT,                  \
	        LIST_SUBSCRIBED,               \
	        "",                            \
	        0,                             \
	        0,                             \
	        ListSubscribedPreSendHook,     \
	        ListSubscribedPostReceiveHook) \
	COMMAND(COMMAND_SUBSCRIBE,             \
	        ROLE_STUDENT,                  \
	        SUBSCRIBE_CLASS,               \
	        "<class>",                     \
	        1,                             \
	        1,                             \
	        SubscribePreSendHook,          \
	        SubscribePostReceiveHook)      \
	COMMAND(COMMAND_CREATE_CLASS,          \
	        ROLE_TEACHER,                  \
	        CREATE_CLASS,                  \
	        "<name> <size>",               \
	        2,                             \
	        2,                             \
	        CreateClassPreSendHook,        \
	        CreateClassPostReceiveHook)    \
	COMMAND(COMMAND_SEND,                  \
	        ROLE_TEACHER,                  \
	        SEND,                          \
	        "<name> <message>",            \
	        2,                             \
	        -1,                            \
	        SendPreSendHook,               \
	        SendPostReceiveHook)

typedef enum {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	ENUM,
	CLASS_COMMANDS
#undef COMMAND
} ClassCommandType;

char *classCommandTypeToString(const ClassCommandType command);
ClassCommandType parseClassCommandType(const char *const string);
int getClassCommandArgsMax(const ClassCommandType command);
int getClassCommandArgsMin(const ClassCommandType command);
Role getClassCommandRole(const ClassCommandType command);
const char *getClassCommandUsageArgs(const ClassCommandType command);

typedef struct ClassCommand {
	ClassCommandType command;
	Vector args;
	Role role;
	char *usageArgs;
	int argsMin;
	int argsMax;
	void (*preSendHook)(const struct ClassCommand command,
	                    TCPSocket *socket);
	void (*postReceiveHook)(const TCPSocket socket);
} ClassCommand;

typedef void (*PreSendHookFunction)(const ClassCommand command,
                                    TCPSocket *socket);
typedef void (*PostReceiveHookFunction)(const TCPSocket socket);
PreSendHookFunction getClassPreSendHook(const ClassCommandType command);
PostReceiveHookFunction getClassPostReceiveHook(const ClassCommandType command);

#define CLASS_COMMAND_FORMAT \
	"{\n"                \
	"  command: %s\n"    \
	"  args: %s\n"       \
	"}"

typedef struct {
	bool valid;
	ClassCommand command;
} ClassCommandOptional;

ClassCommandOptional parseClassCommand(const char *const string);
void printClassCommand(FILE *file, const ClassCommand command);
void *sprintClassCommand(char *string, const ClassCommand command);
void destroyClassCommand(ClassCommand *command);

#endif // !COMMAND_CLASS_H
