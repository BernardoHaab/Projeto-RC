#include "client.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void clientHelpResponseProcessing(const char *const response) {}
void clientLoginResponseProcessing(const char *const response) {}
void clientListClassesResponseProcessing(const char *const response) {}
void clientListSubscribedResponseProcessing(const char *const response) {}
void clientSubscribeResponseProcessing(const char *const response)
{
	char *cloneString = trim(strdup(response));
	Vector args       = vectorStringSplit(cloneString, " ");

	strtok(cloneString, " ");
	char *ipMulticast = strtok(NULL, " ");

	if (ipMulticast == NULL) {
#ifdef DEBUG
		fprintf(stderr, "COMMAND REJECTED\n");
#endif
		free(cloneString);
		return;
	}

	pthread_t thread;
	pthread_create(&thread, NULL, clientMultiCastThread, ipMulticast);
	pthread_join(thread, NULL);

	free(cloneString);
}
void clientCreateClassResponseProcessing(const char *const response) {}
void clientSendResponseProcessing(const char *const response) {}
