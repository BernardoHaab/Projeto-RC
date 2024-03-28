#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector vectorCreate(const size_t elementSize,
                    int (*printElementFunction)(FILE *stream,
                                                const void *const data,
                                                const size_t dataSize))
{
	Vector vector = {
	    .elementSize = elementSize,
	    .size        = 0,
	    .data        = NULL,

	    .printElementFunction = printElementFunction != NULL
	                                ? printElementFunction
	                                : vectorStdMemoryPrint,
	};

	return vector;
}

void vectorClear(Vector *vector)
{
	for (size_t i = 0; i < vector->size; ++i) {
		free(vector->data[i]);
	}
	free(vector->data);

	vector->size = 0;
	vector->data = NULL;
}

void *vectorAdd(Vector *vector, const void *const data)
{
	if (data == NULL) {
		return NULL;
	}

	vector->data
	    = realloc(vector->data, sizeof(vector->data[0]) * ++vector->size);
	vector->data[vector->size - 1]
	    = memcpy(malloc(vector->elementSize), data, vector->elementSize);

	return vector->data[vector->size - 1];
}

void vectorPrint(FILE *file, const Vector vector)
{
	fprintf(file, "[ ");
	for (size_t i = 0; i < vector.size; ++i) {
		if (i > 0) {
			fprintf(file, ", ");
		}

		if (vector.printElementFunction == NULL) {
			vectorStdMemoryPrint(stdout,
			                     vector.data[i],
			                     vector.elementSize);
		} else {
			vector.printElementFunction(stdout,
			                            vector.data[i],
			                            vector.elementSize);
		}
	}
	fprintf(file, " ]\n");
}

Vector vectorFromStringCopy(const char *const string,
                            const char *const delimiter)
{
	Vector vector = vectorCreate(sizeof(char *), vectorStdStringPrint);
	vectorFromString(&vector, string, delimiter);

	return vector;
}

void vectorFromString(Vector *vector,
                      const char *const string,
                      const char *const delimiter)
{
	vectorClear(vector);
	vector->printElementFunction = vectorStdStringPrint;

	const size_t stringLength = strlen(string);
	char *stringClone
	    = strncpy(malloc(sizeof(string[0]) * (stringLength + 1)),
	              string,
	              stringLength);

	for (char *token = strtok(stringClone, delimiter); token != NULL;
	     token       = strtok(NULL, delimiter)) {
		printf("%s\n", token);
		vectorAdd(vector, token);
	}
}

int vectorStdMemoryPrint(FILE *stream,
                         const void *const data,
                         const size_t dataSize)
{
	if (stream == NULL || data == NULL || dataSize <= 0) {
		return 0;
	}

	size_t bytesWritten               = 0;
	const unsigned char *const string = data;

	for (size_t i = 0; i < dataSize; ++i) {
		bytesWritten += fprintf(stream, "%02X", string[i]);
	}

	return bytesWritten;
}

int vectorStdStringPrint(FILE *stream,
                         const void *const data,
                         const size_t dataSize)
{
	(void) dataSize;
	const char *const string = data;
	if (stream == NULL || string == NULL || string[0] == '\0') {
		return 0;
	}

	return fprintf(stream, "\"%s\"", string);
}

#define WRAPPER(type, format, function)                         \
	int function(FILE *stream,                              \
	             const void *const data,                    \
	             const size_t dataSize)                     \
	{                                                       \
		(void) dataSize;                                \
		if (stream == NULL || data == NULL) {           \
			return 0;                               \
		}                                               \
                                                                \
		return fprintf(stream, format, *(type *) data); \
	}
VECTOR_STD_PRIMITIVE_TYPES_PRINT
#undef WRAPPER
