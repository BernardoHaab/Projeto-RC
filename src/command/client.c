#include "command/client.h"

#include "command.h"
#include "command/class.h"
#include "command/client/hooks.h"
#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *classCommandTypeToString(const ClientCommandType command)
{
	assert(0 && "classCommandTypeToString Not implemented yet");
	return NULL;
}

ClientCommandType parseClientCommandType(const char *const string)
{
	if (string == NULL || *string == '\0') {
		return COMMAND_HELP;
	}

#define COMMAND(ENUM,                             \
                ROLE,                             \
                BASE_COMMAND,                     \
                USAGE_ARGS,                       \
                ARGS_MIN,                         \
                ARGS_MAX,                         \
                PRE_SEND_HOOK_SUFFIX,             \
                POST_RECEIVED_HOOK_SUFFIX)        \
	if (strcmp(string, #BASE_COMMAND) == 0) { \
		return ENUM;                      \
	}
	CLIENT_COMMANDS
#undef COMMAND

	// TODO: assert(0 && "Unknown command type");

	return COMMAND_HELP;
}

int getClientCommandArgsMax(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return ARGS_MAX;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return 0;
}

int getClientCommandArgsMin(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return ARGS_MIN;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return 0;
}

Role getClientCommandRole(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return ROLE;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return ROLE_GUEST;
}

const char *getClientCommandUsageArgs(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return USAGE_ARGS;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

PreSendHookFunction getClassPreSendHook(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return client##PRE_SEND_HOOK_SUFFIX;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

PostReceiveHookFunction getClassPostReceiveHook(const ClientCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,                      \
                ROLE,                      \
                BASE_COMMAND,              \
                USAGE_ARGS,                \
                ARGS_MIN,                  \
                ARGS_MAX,                  \
                PRE_SEND_HOOK_SUFFIX,      \
                POST_RECEIVED_HOOK_SUFFIX) \
	case ENUM:                         \
		return client##POST_RECEIVED_HOOK_SUFFIX;
		CLIENT_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

ClientCommandOptional parseClientCommand(const char *const string)
{
	static const ClientCommandOptional nullopt = {0};

	if (string == NULL || *string == '\0') {
		return nullopt;
	}

	Vector args = vectorStringSplit(string, COMMAND_DELIMITER);

	const ClientCommandType baseCommand
	    = parseClientCommandType(*(char **) vectorGet(&args, 0));

	const ClientCommandOptional command = {
	    .valid   = true,
	    .command = {
		    .command = baseCommand,
		    .args = args,
		    .role = getClientCommandRole(baseCommand),
		    .usageArgs = (char *) getClientCommandUsageArgs(baseCommand),
		    .argsMin = getClientCommandArgsMin(baseCommand),
		    .argsMax = getClientCommandArgsMax(baseCommand),
		    .preSendHook = getClassPreSendHook(baseCommand),
		    .postReceiveHook = getClassPostReceiveHook(baseCommand),
	    },
	};

	return command;
}

void printClientCommand(FILE *file, const ClientCommand command)
{
	fprintf(file,
	        CLASS_COMMAND_FORMAT "\n",
	        classCommandTypeToString(command.command),
	        vectorString(command.args));
}

void *sprintClientCommand(char *string, const ClientCommand command)
{
	if (string == NULL) {
		return NULL;
	}

	sprintf(string,
	        CLASS_COMMAND_FORMAT "\n",
	        classCommandTypeToString(command.command),
	        vectorString(command.args));

	return string;
}

void destroyClientCommand(ClientCommand *command)
{
	vectorClear(&command->args);

	memset(command, 0, sizeof(*command));
}
