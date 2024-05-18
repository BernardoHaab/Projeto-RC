#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

#include "udp/socket.h"
#include "vector.h"

#include <stdbool.h>
#include <stdio.h>

#define CONFIG_COMMANDS_NOT_LOGGED_IN      \
	COMMAND(CONFIG_HELP,               \
	        help,                      \
	        "",                        \
	        -1,                        \
	        -1,                        \
	        configHelpPreSendHook,     \
	        configHelpPostReceiveHook) \
	COMMAND(CONFIG_LOGIN,              \
	        LOGIN,                     \
	        "<username> <password>",   \
	        2,                         \
	        2,                         \
	        configLoginPreSendHook,    \
	        configLoginPostReceiveHook)

#define CONFIG_COMMANDS_LOGGED_IN                                        \
	COMMAND(CONFIG_ADD_USER,                                         \
	        ADD_USER,                                                \
	        "<username> <password> <administrator/aluno/professor>", \
	        3,                                                       \
	        3,                                                       \
	        configAddUserPreSendHook,                                \
	        configAddUserPostReceiveHook)                            \
	COMMAND(CONFIG_DELETE_USER,                                      \
	        DEL,                                                     \
	        "<username>",                                            \
	        1,                                                       \
	        1,                                                       \
	        configDeleteUserPreSendHook,                             \
	        configDeleteUserPostReceiveHook)                         \
	COMMAND(CONFIG_LIST_USERS,                                       \
	        LIST,                                                    \
	        "",                                                      \
	        0,                                                       \
	        0,                                                       \
	        configListUsersPreSendHook,                              \
	        configListUsersPostReceiveHook)                          \
	COMMAND(CONFIG_QUIT_SERVER,                                      \
	        QUIT_SERVER,                                             \
	        "",                                                      \
	        0,                                                       \
	        0,                                                       \
	        configQuitServerPreSendHook,                             \
	        configQuitServerPostReceiveHook)

#define CONFIG_COMMANDS               \
	CONFIG_COMMANDS_NOT_LOGGED_IN \
	CONFIG_COMMANDS_LOGGED_IN

typedef enum {
#define COMMAND(ENUM,              \
                COMMAND,           \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	ENUM,
	CONFIG_COMMANDS
#undef COMMAND
} ConfigCommandType;

char *configCommandTypeToString(const ConfigCommandType command);
ConfigCommandType parseConfigCommandType(const char *const string);
int getArgsMaxFromConfigCommandType(const ConfigCommandType command);
int getArgsMinFromConfigCommandType(const ConfigCommandType command);
const char *getUsageArgsFromConfigCommand(const ConfigCommandType command);

typedef struct ConfigCommand {
	ConfigCommandType command;
	Vector args;
	char *usageArgs;
	int argsMin;
	int argsMax;
	void (*preSendHook)(const struct ConfigCommand command,
	                    UDPSocket *socket);
	void (*postReceiveHook)(const char *const response);
} ConfigCommand;

typedef void (*PreSendHookFunction)(const ConfigCommand command,
                                    UDPSocket *socket);
typedef void (*PostReceiveHookFunction)(const char *const response);
PreSendHookFunction
getPreSendHookFromConfigCommand(const ConfigCommandType command);
PostReceiveHookFunction
getPostReceiveHookFromConfigCommand(const ConfigCommandType command);

#define CONFIG_COMMAND_FORMAT \
	"{\n"                 \
	"  command: %s\n"     \
	"  args: %s\n"        \
	"}"

typedef struct {
	bool valid;
	ConfigCommand command;
} ConfigCommandOptional;

ConfigCommandOptional parseConfigCommand(const char *const string);
void printConfigCommand(FILE *file, const ConfigCommand command);
void *sprintConfigCommand(char *string, const ConfigCommand command);
void destroyConfigCommand(ConfigCommand *command);

#endif // !COMMAND_CONFIG_H
