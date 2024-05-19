#ifndef COMMAND_CLIENT_H
#define COMMAND_CLIENT_H

#include "command.h"
#include "command/class.h"
#include "tcp/socket.h"

#define CLIENT_COMMANDS CLASS_COMMANDS

typedef ClassCommandType ClientCommandType;
typedef ClassCommand ClientCommand;

typedef void (*PreSendHookFunction)(const ClientCommand command,
                                    TCPSocket *socket);
typedef void (*PostReceiveHookFunction)(const TCPSocket socket);
PreSendHookFunction getClientPreSendHook(const ClientCommandType command);
PostReceiveHookFunction
getClientPostReceiveHook(const ClientCommandType command);

#define CLIENT_COMMAND_FORMAT CLASS_COMMAND_FORMAT

typedef ClassCommandOptional ClientCommandOptional;

ClientCommandOptional parseClientCommand(const char *const string);
void printClientCommand(FILE *file, const ClientCommand command);
void *sprintClientCommand(char *string, const ClientCommand command);
void destroyClientCommand(ClientCommand *command);

#endif // !COMMAND_CLIENT_H
