#include "command/class.h"

#include "command.h"
#include "utils.h"
#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *classCommandTypeToString(const ClassCommandType command)
{
	assert(0 && "classCommandTypeToString Not implemented yet");
	return NULL;
}

ClassCommandType parseClassCommandType(const char *const string)
{
	assert(0 && "parseClassCommandType Not implemented yet");
	return (ClassCommandType) -1;
}

int getClassCommandArgsMax(const ClassCommandType command)
{
	assert(0 && "getClassCommandArgsMax Not implemented yet");
	return 0;
}

int getClassCommandArgsMin(const ClassCommandType command)
{
	assert(0 && "getClassCommandArgsMin Not implemented yet");
	return 0;
}

Role getClassCommandRole(const ClassCommandType command)
{
	assert(0 && "getClassCommandRole Not implemented yet");
	return (Role) -1;
}

const char *getClassCommandUsageArgs(const ClassCommandType command)
{
	assert(0 && "getClassCommandUsageArgs Not implemented yet");
	return NULL;
}

ClassCommandOptional parseClassCommand(const char *const string)
{
	static const ClassCommandOptional nullopt = {0};

	if (string == NULL || *string == '\0') {
		return nullopt;
	}

	char *cloneString = trim(strdup(string));

	Vector args = vectorStringSplit(cloneString, COMMAND_DELIMITER);
	free(cloneString);

	const ClassCommandOptional command = {
	    .valid   = true,
	    .command = {
		    .command = parseClassCommandType(*(char **) vectorGet(&args, 0)),
		    .args = args,
		    .preSendHook = NULL,
		    .postReceiveHook = NULL,
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
