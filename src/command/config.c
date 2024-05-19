#include "command/config.h"

#include "command.h"
#include "command/config.h"
#include "command/config/hooks.h"
#include "utils.h"
#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *configCommandTypeToString(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return #BASE_COMMAND;
		CONFIG_COMMANDS
#undef COMMAND
	}

	return NULL;
}

ConfigCommandType parseConfigCommandType(const char *const string)
{
#define COMMAND(ENUM,                             \
                BASE_COMMAND,                     \
                USAGE_ARGS,                       \
                ARGS_MIN,                         \
                ARGS_MAX,                         \
                PRE_SEND_HOOK,                    \
                POST_RECEIVE_HOOK)                \
	if (strcmp(string, #BASE_COMMAND) == 0) { \
		return ENUM;                      \
	}
	CONFIG_COMMANDS
#undef COMMAND

	return CONFIG_HELP;
}

int getArgsMaxFromConfigCommandType(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return ARGS_MAX;
		CONFIG_COMMANDS
#undef COMMAND
	default:
		assert(0 && "Unknown command type");
	}

	return 0;
}

int getArgsMinFromConfigCommandType(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return ARGS_MIN;
		CONFIG_COMMANDS
#undef COMMAND
	default:
		assert(0 && "Unknown command type");
	}

	return 0;
}

const char *getUsageArgsFromConfigCommand(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return USAGE_ARGS;
		CONFIG_COMMANDS
#undef COMMAND
	default:
		assert(0 && "Unknown command type");
	}

	return NULL;
}

PreSendHookFunction getConfigPreSendHook(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return PRE_SEND_HOOK;
		CONFIG_COMMANDS
#undef COMMAND
	}

	return NULL;
}

PostReceiveHookFunction getConfigPostReceiveHook(const ConfigCommandType command)
{
	switch (command) {
#define COMMAND(ENUM,              \
                BASE_COMMAND,      \
                USAGE_ARGS,        \
                ARGS_MIN,          \
                ARGS_MAX,          \
                PRE_SEND_HOOK,     \
                POST_RECEIVE_HOOK) \
	case ENUM:                 \
		return POST_RECEIVE_HOOK;
		CONFIG_COMMANDS
#undef COMMAND
	}

	return NULL;
}

ConfigCommandOptional parseConfigCommand(const char *const string)
{
	static const ConfigCommandOptional nullopt = {0};

	if (string == NULL || *string == '\0') {
		return nullopt;
	}

	char *cloneString = trim(strdup(string));

	Vector args = vectorStringSplit(cloneString, COMMAND_DELIMITER);
	free(cloneString);

	const ConfigCommandType commandType
	    = parseConfigCommandType(*(char **) vectorGet(&args, 0));

	const ConfigCommandOptional command = {
	    .valid   = true,
	    .command = {
		    .command = commandType,
		    .args = args,
		    .argsMax = getArgsMaxFromConfigCommandType(commandType),
		    .argsMin = getArgsMinFromConfigCommandType(commandType),
		    .usageArgs = (char *) getUsageArgsFromConfigCommand(commandType),
		    .preSendHook = getConfigPreSendHook(commandType),
		    .postReceiveHook = getConfigPostReceiveHook(commandType),
	    },
	};

	return command;
}

void printConfigCommand(FILE *file, const ConfigCommand command)
{
	fprintf(file,
	        CONFIG_COMMAND_FORMAT "\n",
	        configCommandTypeToString(command.command),
	        vectorString(command.args));
}

void *sprintConfigCommand(char *string, const ConfigCommand command)
{
	if (string == NULL) {
		return NULL;
	}

	sprintf(string,
	        CONFIG_COMMAND_FORMAT "\n",
	        configCommandTypeToString(command.command),
	        vectorString(command.args));

	return string;
}

void destroyConfigCommand(ConfigCommand *command)
{
	vectorClear(&command->args);

	memset(command, 0, sizeof(*command));
}
