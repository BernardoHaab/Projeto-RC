#ifndef COMMAND_CLASS_HOOKS_H
#define COMMAND_CLASS_HOOKS_H

#include "command/class.h"

#define COMMAND(ENUM,                                                \
                ROLE,                                                \
                BASE_COMMAND,                                        \
                USAGE_ARGS,                                          \
                ARGS_MIN,                                            \
                ARGS_MAX,                                            \
                PRE_SEND_HOOK_SUFFIX,                                \
                POST_RECEIVED_HOOK_SUFFIX)                           \
	void class##PRE_SEND_HOOK_SUFFIX(const ClassCommand command, \
	                                 TCPSocket *socket);         \
	void class##POST_RECEIVED_HOOK_SUFFIX(const TCPSocket socket);
CLASS_COMMANDS
#undef COMMAND

#endif // !COMMAND_CLASS_HOOKS_H
