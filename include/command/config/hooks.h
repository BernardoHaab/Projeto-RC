#ifndef CONFIG_HOOKS_H
#define CONFIG_HOOKS_H

#include "command/config.h"

#define COMMAND(ENUM,                                                       \
                COMMAND,                                                    \
                USAGE_ARGS,                                                 \
                ARGS_MIN,                                                   \
                ARGS_MAX,                                                   \
                PRE_SEND_HOOK,                                              \
                POST_RECEIVE_HOOK)                                          \
	void PRE_SEND_HOOK(const ConfigCommand command, UDPSocket *socket); \
	void POST_RECEIVE_HOOK(const char *const response);
CONFIG_COMMANDS
#undef COMMAND

#endif // !CONFIG_HOOKS_H
