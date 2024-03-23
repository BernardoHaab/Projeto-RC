#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

#define COMMAND_ENUM                         \
  WRAPPER(HELP, "help", "help", commandHelp) \
  WRAPPER(ADD_USER, "ADD_USERR", "ADD_USER <username> <password> <administrador/aluno/professor> ", addUser)
typedef enum
{
#define WRAPPER(enum, text, usage, function) enum,
  COMMAND_ENUM
#undef WRAPPER
} Command;

void processUdp();

Command processCommand(
    const char *const buffer,
    const size_t bufferSize,
    char *response);

void addUser(const char *const argument, char *response);
void commandHelp(const char *const argument, char *response);

#endif // !COMMAND_H
