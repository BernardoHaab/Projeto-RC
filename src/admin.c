#include "admin.h"

#include "command/config.h"
#include "debug.h"
#include "udp/socket.h"
#include "vector.h"

#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

Vector loggedAdmins = {0};

void setupAdminConsole(UDPSocket *serverUDPSocket)
{
	loggedAdmins
	    = vectorCreate(sizeof(struct sockaddr_in), NULL, printAdminIP);

	debugMessage(stdout, OK, "Started Admin Console\n");

	ConfigCommand command = {0};
	do {
		destroyConfigCommand(&command);

		UDPSocket clientUDPSocket
		    = receiveFromUDPSocket(serverUDPSocket);

		debugMessage(stdout,
		             INFO,
		             "Received: \"%s\" from ",
		             serverUDPSocket->buffer);
		printSocketIP(stdout, true, clientUDPSocket.address);

		const ConfigCommandOptional commandOpt
		    = parseConfigCommand(serverUDPSocket->buffer);

		command = commandOpt.command;

		if (!commandOpt.valid) {
			destroyConfigCommand(&command);
			continue;
		}

		printConfigCommand(stdout, command);

		const PostReceiveHookResponse response
		    = command.postReceiveHook(clientUDPSocket);
		if (!response.valid) {
			destroyConfigCommand(&command);
			debugMessage(stdout, ERROR, "%s\n", response.reason);
			writeToUDPSocket(&clientUDPSocket, response.reason);
			continue;
		}

		command.preSendHook(command, &clientUDPSocket);

		sendToUDPSocket(&clientUDPSocket);
		fflush(stdout);
	} while (command.command != CONFIG_QUIT_SERVER);

	// TODO: Corrigir o close do socket?
	closeUDPSocket(serverUDPSocket);
}


bool isLogged(Vector *loggedAdmins, const struct sockaddr_in clientIP)
{
	for (size_t i = 0; i < loggedAdmins->size; ++i) {
		const struct sockaddr_in currentLoggedAdmin
		    = *(struct sockaddr_in *) vectorGet(loggedAdmins, i);

		if (socketAdressEqual(currentLoggedAdmin, clientIP)) {
			return true;
		}
	}

	return false;
}

void addLogin(Vector *loggedAdmins, const struct sockaddr_in clientIP)
{
	if (loggedAdmins == NULL || isLogged(loggedAdmins, clientIP)) {
		return;
	}

	vectorPushBack(loggedAdmins, &clientIP);

	vectorPrint(stdout, *loggedAdmins);
}

void printAdminIP(FILE *file, const void *const data, const size_t size)
{
	(void) size;

	if (file == NULL || data == NULL) {
		return;
	}

	const struct sockaddr_in admin = *(struct sockaddr_in *) data;
	fprintf(file, "%s", inet_ntoa(admin.sin_addr));
}

bool socketAdressEqual(const struct sockaddr_in a, const struct sockaddr_in b)
{
	return a.sin_addr.s_addr == b.sin_addr.s_addr
	       && a.sin_port == b.sin_port;
}
