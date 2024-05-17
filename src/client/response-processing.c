#include "client.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

extern sem_t promptSemaphore;

void clientHelpResponseProcessing(const char *const response) {}
void clientLoginResponseProcessing(const char *const response) {}
void clientListClassesResponseProcessing(const char *const response) {}
void clientListSubscribedResponseProcessing(const char *const response) {}

void clientSubscribeResponseProcessing(const char *const response)
{
	char *cloneString = trim(strdup(response));
	Vector args       = vectorStringSplit(cloneString, " ");
	free(cloneString);

	pthread_t thread;
	sem_wait(&promptSemaphore);
	pthread_create(&thread,
	               NULL,
	               clientMultiCastThread,
	               *(char **) vectorGet(&args, 1));
}

void clientCreateClassResponseProcessing(const char *const response) {}
void clientSendResponseProcessing(const char *const response) {}
