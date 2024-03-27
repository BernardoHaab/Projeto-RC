#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

#define COMMAND_ENUM                                                                                        \
  WRAPPER(HELP, "help", "help", commandHelp)                                                                \
  WRAPPER(ADD_USER, "ADD_USER", "ADD_USER <username> <password> <administrador/aluno/professor> ", addUser) \
  WRAPPER(DELETE_USER, "DELETE_USER", "DELETE_USER <username>", deleteUser)                                 \
  WRAPPER(LIST_USERS, "LIST_USERS", "LIST_USERS", listUsers)                                                \
  WRAPPER(QUIT, "QUIT", "QUIT", quitServer)
typedef enum
{
#define WRAPPER(enum, text, usage, function) enum,
  COMMAND_ENUM
#undef WRAPPER
} Command;

Command processCommand(
    const char *const buffer,
    const size_t bufferSize,
    char *response);

void addUser(const char *const argument, char *response);
void deleteUser(const char *const argument, char *response);
void listUsers(const char *const argument, char *response);
void quitServer(const char *const argument, char *response);
void commandHelp(const char *const argument, char *response);

#endif // !COMMAND_H
