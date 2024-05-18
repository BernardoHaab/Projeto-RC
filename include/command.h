#ifndef COMMAND_H
#define COMMAND_H

#include "vector.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define ROLES                                \
	ROLE(ROLE_ADMIN, "administrator", 1) \
	ROLE(ROLE_STUDENT, "aluno", 2)       \
	ROLE(ROLE_TEACHER, "professor", 3)

typedef enum {
#define ROLE(ENUM, STRING, BIT_SHIFT) ENUM = 1 << BIT_SHIFT,
	ROLES
#undef ROLE

	    ROLE_GUEST
	= 0
#define ROLE(ENUM, STRING, BIT_SHIFT) | ENUM
	ROLES
#undef ROLE
	,
} Role;

Role parseRole(const char *const string);
char *getStringFromRole(const Role role);

#define COMMAND_DELIMITER " \n"

#define COMMAND_REPLY_OK       "OK"
#define COMMAND_REPLY_ACCEPTED "ACCEPTED"
#define COMMAND_REPLY_REJECTED "REJECTED"

#endif // !COMMAND_H
