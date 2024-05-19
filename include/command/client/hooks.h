#ifndef COMMAND_CLIENT_HOOKS_H
#define COMMAND_CLIENT_HOOKS_H

#include "command/client.h"

#define COMMAND(ENUM,                                                 \
                ROLE,                                                 \
                BASE_COMMAND,                                         \
                USAGE_ARGS,                                           \
                ARGS_MIN,                                             \
                ARGS_MAX,                                             \
                PRE_SEND_HOOK_SUFFIX,                                 \
                POST_RECEIVED_HOOK_SUFFIX)                            \
	void client##PRE_SEND_HOOK_SUFFIX(const ClassCommand command, \
	                                  TCPSocket *socket);         \
	void client##POST_RECEIVED_HOOK_SUFFIX(const TCPSocket socket);
CLIENT_COMMANDS
#undef COMMAND

#endif // !COMMAND_CLIENT_HOOKS_H
