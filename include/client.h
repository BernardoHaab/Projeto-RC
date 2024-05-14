#ifndef CLIENT_H
#define CLIENT_H

#include "vector.h"

#define EXIT_COMMAND "EXIT"

#define CLIENT_COMMANDS                                \
	CLIENT(CLIENT_HELP,                            \
	       "help",                                 \
	       -1,                                     \
	       -1,                                     \
	       clientHelpInputProcessing,              \
	       clientHelpResponseProcessing)           \
	CLIENT(CLIENT_LOGIN,                           \
	       "LOGIN <username> <password>",          \
	       2,                                      \
	       2,                                      \
	       clientLoginInputProcessing,             \
	       clientLoginResponseProcessing)          \
	CLIENT(CLIENT_LIST_CLASSES,                    \
	       "LIST_CLASSES",                         \
	       0,                                      \
	       0,                                      \
	       clientListClassesInputProcessing,       \
	       clientListClassesResponseProcessing)    \
	CLIENT(CLIENT_LIST_SUBSCRIBED,                 \
	       "LIST_SUBSCRIBED",                      \
	       0,                                      \
	       0,                                      \
	       clientListSubscribedInputProcessing,    \
	       clientListSubscribedResponseProcessing) \
	CLIENT(CLIENT_SUBSCRIBE,                       \
	       "SUBSCRIBE_CLASS <class>",              \
	       1,                                      \
	       1,                                      \
	       clientSubscribeInputProcessing,         \
	       clientSubscribeResponseProcessing)      \
	CLIENT(CLIENT_CREATE_CLASS,                    \
	       "CREATE_CLASS <name> <size>",           \
	       2,                                      \
	       2,                                      \
	       clientCreateClassInputProcessing,       \
	       clientCreateClassResponseProcessing)    \
	CLIENT(CLIENT_SEND,                            \
	       "SEND <name> <message>",                \
	       2,                                      \
	       -1,                                     \
	       clientSendInputProcessing,              \
	       clientSendResponseProcessing)

typedef enum {
#define CLIENT(ENUM,                \
               USAGE,               \
               ARGS_MIN,            \
               ARGS_MAX,            \
               INPUT_PROCESSING,    \
               RESPONSE_PROCESSING) \
	ENUM,
	CLIENT_COMMANDS
#undef CLIENT
} ClientCommandType;

ClientCommandType parseClientCommandType(const char *const string);
char *clientCommandTypeToString(const ClientCommandType command);

typedef struct {
	ClientCommandType command;
	Vector args;
} ClientCommand;

#define CLIENT_COMMAND_FORMAT \
	"{\n"                 \
	"  command: %s\n"     \
	"  args: %s\n"        \
	"}"

char *clientCommandToString(const ClientCommand command);
ClientCommand parseClientCommand(const char *const string);
void printClientCommand(FILE *file, const ClientCommand command);

#define CLIENT(ENUM,                                          \
               USAGE,                                         \
               ARGS_MIN,                                      \
               ARGS_MAX,                                      \
               INPUT_PROCESSING,                              \
               RESPONSE_PROCESSING)                           \
	void INPUT_PROCESSING(const ClientCommand command);   \
	void RESPONSE_PROCESSING(const char *const response); \
	CLIENT_COMMANDS
#undef CLIENT

void usage(const char *const programName);

#endif // !CLIENT_H
