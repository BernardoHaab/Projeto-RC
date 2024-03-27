#include "debug.h"

#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void printSocketIP(FILE *const file,
                   const bool brief,
                   const struct sockaddr_in address)
{
	char ipAddressString[INET_ADDRSTRLEN] = {[0] = '\0'};
	fprintf(file,
	        brief ? IP_FORMAT_BRIEF : IP_FORMAT,
	        inet_ntop(AF_INET,
	                  &address.sin_addr,
	                  ipAddressString,
	                  INET_ADDRSTRLEN),
	        ntohs(address.sin_port));
}

void debugMessage(FILE *const file,
                  const DebugMessageType type,
                  const char *const format,
                  ...)
{
	// BUG: debugCounter is a static variable but is not shared between
	// processes leading to incongruent log counts. Fix: shared memory.
	static size_t debugCounter                            = 1;
	char typeString[DEBUG_MESSAGE_TYPE_STRING_MAX_LENGTH] = {[0] = '\0'};
	char colorANSI[ANSI_LENGTH]                           = {[0] = '\0'};

	va_list args;
	va_start(args, format);

	switch (type) {
#define WRAPPER(type, color)                                   \
	case (type): {                                         \
		strncpy(typeString,                            \
		        #type,                                 \
		        DEBUG_MESSAGE_TYPE_STRING_MAX_LENGTH); \
		strncpy(colorANSI, color, ANSI_LENGTH);        \
		break;                                         \
	}
		DEBUG_MESSAGE_TYPE_ENUM
#undef WRAPPER
	}

	static const int maxLen = DEBUG_MESSAGE_TYPE_STRING_MAX_LENGTH;
	const int halfLength    = strnlen(typeString, maxLen) / 2;
	const int leftSpace     = maxLen / 2 + halfLength;
	const int rightSpace    = maxLen / 2 - halfLength;
	fprintf(file,
	        "[%03zu]%s[%*s%*s]" RESET ": ",
	        debugCounter++,
	        colorANSI,
	        leftSpace,
	        typeString,
	        rightSpace,
	        "");
	vfprintf(file, format, args);

	va_end(args);
}

void error(const char *const message)
{
	debugMessage(stderr, ERROR, "%s", message);
	perror(message);
	exit(EXIT_FAILURE);
}
