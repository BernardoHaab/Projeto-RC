#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <unistd.h>

#define CLI_COMMAND_DELIMITER " "

typedef struct {
	char **args;
	size_t nargs;
} CliCommand;

void parseCliCommand(const char *const string, CliCommand *cliCommand);
CliCommand parseCliCommandCopy(const char *const string);
void printCliCommand(FILE *file, const CliCommand cliCommand);
void *sprintCliCommand(char *string, const CliCommand cliCommand);
void destroyCliCommand(CliCommand *cliCommand);

#endif // !COMMAND_H
