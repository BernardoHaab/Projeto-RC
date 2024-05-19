#ifndef CLIENT_H
#define CLIENT_H

#include "command.h"
#include "vector.h"

#define EXIT_COMMAND "EXIT"

void usage(const char *const programName);
void *clientMultiCastThread(void *multicastIP);

#endif // !CLIENT_H
