#include "command.h"
#include "command/config.h"
#include "command/config/hooks.h"
#include "udp/socket.h"

#include <stdbool.h>

static const PostReceiveHookResponse validPostReceiveHookResponse = {
    .valid  = true,
    .reason = "",
};

#define INVALID_POST_RECEIVE_HOOK_RESPONSE(REASON) \
	((PostReceiveHookResponse){                \
	    .valid  = false,                       \
	    .reason = REASON,                      \
	})

static PostReceiveHookResponse
configDefaultPostReceiveHook(const UDPSocket socket)
{
	const ConfigCommandOptional commandOpt
	    = parseConfigCommand(socket.buffer);

	if (!commandOpt.valid) {
		return INVALID_POST_RECEIVE_HOOK_RESPONSE("Unknown command");
	}

	const ConfigCommand command = commandOpt.command;

	if (!validNumberOfArgs(command.argsMin,
	                       command.argsMax,
	                       command.args.size - 1)) {
		return INVALID_POST_RECEIVE_HOOK_RESPONSE(
		    "Invalid number of arguments");
	}

	return validPostReceiveHookResponse;
}

PostReceiveHookResponse configHelpPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}

PostReceiveHookResponse configLoginPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}

PostReceiveHookResponse configAddUserPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}

PostReceiveHookResponse configDeleteUserPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}

PostReceiveHookResponse configListUsersPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}

PostReceiveHookResponse configQuitServerPostReceiveHook(const UDPSocket socket)
{
	return configDefaultPostReceiveHook(socket);
}
