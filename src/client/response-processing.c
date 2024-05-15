#include "client.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
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
	free(cloneString);

	const char *const command = *(char **) vectorGet(&args, 0);
	if (strncmp(command, REPLY_ACCEPTED, sizeof(REPLY_ACCEPTED)) != 0
	    || args.size < 2) {
#ifdef DEBUG
		fprintf(stderr, "COMMAND REJECTED\n");
#endif
		vectorClear(&args);
		return;
	}

	pthread_t thread;
	pthread_create(&thread,
	               NULL,
	               clientMultiCastThread,
	               vectorGet(&args, 1));
	pthread_join(thread, NULL);

	vectorClear(&args);
}
void clientCreateClassResponseProcessing(const char *const response) {}
void clientSendResponseProcessing(const char *const response) {}
