#include "command.h"

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parseCliCommand(const char *const string, CliCommand *cliCommand)
{
	char *cloneString = trim(strdup(string));

	destroyCliCommand(cliCommand);

	for (char *token = strtok(cloneString, CLI_COMMAND_DELIMITER);
	     token != NULL;
	     token = strtok(NULL, CLI_COMMAND_DELIMITER)) {
		cliCommand->args = realloc(cliCommand->args,
		                           sizeof(cliCommand->args[0])
		                               * ++cliCommand->nargs);

		cliCommand->args[cliCommand->nargs - 1] = strdup(token);
	}

	free(cloneString);
}

CliCommand parseCliCommandCopy(const char *const string)
{
	CliCommand cliCommand = {0};

	parseCliCommand(string, &cliCommand);

	return cliCommand;
}

void printCliCommand(FILE *file, const CliCommand cliCommand)
{
	fprintf(file,
	        "CliCommand: {\n"
	        "\tnargs: %zu\n",
	        cliCommand.nargs);

	for (size_t i = 0; i < cliCommand.nargs; ++i) {
		const char *const arg = cliCommand.args[i];
		fprintf(file, "\tArgument %zu: %s\n", i, arg);
	}

	fprintf(file, "}\n");
}

void *sprintCliCommand(char *string, const CliCommand cliCommand)
{
	int off = 0;

	off += sprintf(string,
	               "CliCommand: {\n"
	               "\tnargs: %zu\n",
	               cliCommand.nargs);

	for (size_t i = 0; i < cliCommand.nargs; ++i) {
		const char *const arg = cliCommand.args[i];
		off += sprintf(string + off, "\tArgument %zu: %s\n", i, arg);
	}

	sprintf(string + off, "}\n");

	return string;
}

void destroyCliCommand(CliCommand *cliCommand)
{
	for (size_t i = 0; i < cliCommand->nargs; ++i) {
		free(cliCommand->args[i]);
	}

	cliCommand->args  = NULL;
	cliCommand->nargs = 0;
}
