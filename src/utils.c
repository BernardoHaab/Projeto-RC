#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim(char *string)
{
	size_t length = 0;
	char *frontP  = string;
	char *endP    = NULL;

	if (string == NULL) {
		return NULL;
	}
	if (string[0] == '\0') {
		return string;
	}

	length = strlen(string);
	endP   = string + length;

	/* Move the front and back pointers to address the first non-whitespace
   * characters from each end.
   */
	while (isspace((unsigned char) *frontP)) {
		++frontP;
	}
	if (endP != frontP) {
		while (isspace((unsigned char) *(--endP)) && endP != frontP) {}
	}

	if (frontP != string && endP == frontP) *string = '\0';
	else if (string + length - 1 != endP) *(endP + 1) = '\0';

	/* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
	endP = string;
	if (frontP != string) {
		while (*frontP) {
			*endP++ = *frontP++;
		}
		*endP = '\0';
	}

	return string;
}

size_t countChar(const char *const string, const char character)
{
	size_t count = 0;

	for (char *s = (char *) string; *s != '\0'; s++) {
		if (*s != character) continue;

		++count;
	}

	return count;
}

size_t
countNChar(const char *const string, const char character, const size_t nBytes)
{
	size_t count = 0;

	size_t index = 0;
	for (char *s = (char *) string; index < nBytes && *s != '\0';
	     s++, index++) {
		if (*s != character) continue;

		++count;
	}

	return count;
}
