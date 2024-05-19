#include "command/class.h"

#include "command.h"
#include "command/class/hooks.h"
#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *classCommandTypeToString(const ClassCommandType command)
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
		return #BASE_COMMAND;
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

ClassCommandType parseClassCommandType(const char *const string)
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
	CLASS_COMMANDS
#undef COMMAND

	// TODO: assert(0 && "Unknown command type");

	return COMMAND_HELP;
}

int getClassCommandArgsMax(const ClassCommandType command)
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
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return 0;
}

int getClassCommandArgsMin(const ClassCommandType command)
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
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return 0;
}

Role getClassCommandRole(const ClassCommandType command)
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
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return ROLE_GUEST;
}

const char *getClassCommandUsageArgs(const ClassCommandType command)
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
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

PreSendHookFunction getClassPreSendHook(const ClassCommandType command)
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
		return class##PRE_SEND_HOOK_SUFFIX;
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

PostReceiveHookFunction getClassPostReceiveHook(const ClassCommandType command)
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
		return class##POST_RECEIVED_HOOK_SUFFIX;
		CLASS_COMMANDS
#undef COMMAND
	}

	assert(0 && "Unknown command type");

	return NULL;
}

ClassCommandOptional parseClassCommand(const char *const string)
{
	static const ClassCommandOptional nullopt = {0};

	if (string == NULL || *string == '\0') {
		return nullopt;
	}

	Vector args = vectorStringSplit(string, COMMAND_DELIMITER);

	const ClassCommandType baseCommand
	    = parseClassCommandType(*(char **) vectorGet(&args, 0));

	const ClassCommandOptional command = {
	    .valid   = true,
	    .command = {
		    .command = baseCommand,
		    .args = args,
		    .role = getClassCommandRole(baseCommand),
		    .usageArgs = (char *) getClassCommandUsageArgs(baseCommand),
		    .argsMin = getClassCommandArgsMin(baseCommand),
		    .argsMax = getClassCommandArgsMax(baseCommand),
		    .preSendHook = getClassPreSendHook(baseCommand),
		    .postReceiveHook = getClassPostReceiveHook(baseCommand),
	    },
	};

	return command;
}

void printClassCommand(FILE *file, const ClassCommand command)
{
	fprintf(file,
	        CLASS_COMMAND_FORMAT "\n",
	        classCommandTypeToString(command.command),
	        vectorString(command.args));
}

void *sprintClassCommand(char *string, const ClassCommand command)
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

void destroyClassCommand(ClassCommand *command)
{
	vectorClear(&command->args);

	memset(command, 0, sizeof(*command));
}
