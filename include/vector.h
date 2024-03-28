#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <unistd.h>

typedef struct Vector {
	size_t size;
	size_t elementSize;
	void **data;

	int (*printElementFunction)(FILE *stream,
	                            const void *const data,
	                            const size_t dataSize);
} Vector;

Vector vectorCreate(const size_t elementSize,
                    int (*printElementFunction)(FILE *stream,
                                                const void *const data,
                                                const size_t dataSize));
void vectorClear(Vector *vector);
void *vectorAdd(Vector *vector, const void *const data);
void vectorPrint(FILE *file, const Vector vector);
void vectorFromString(Vector *vector,
                      const char *const string,
                      const char *const delimiter);
Vector vectorFromStringCopy(const char *const string,
                            const char *const delimiter);

#define VECTOR_STD_PRIMITIVE_TYPES_PRINT                         \
	WRAPPER(unsigned int, "%u", vectorStdUnsignedIntPrint) \
	WRAPPER(int, "%d", vectorStdIntPrint)                  \
	WRAPPER(size_t, "%zu", vectorStdSizeTPrint)

int vectorStdMemoryPrint(FILE *stream,
                         const void *const data,
                         const size_t dataSize);
int vectorStdStringPrint(FILE *stream,
                         const void *const data,
                         const size_t dataSize);

#define WRAPPER(type, format, function)      \
	int function(FILE *stream,           \
	             const void *const data, \
	             const size_t dataSize);
VECTOR_STD_PRIMITIVE_TYPES_PRINT
#undef WRAPPER

#endif // !VECTOR_H
