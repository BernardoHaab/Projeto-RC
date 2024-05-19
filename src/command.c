#include "command.h"

#include <assert.h>
#include <string.h>

Role parseRole(const char *const string)
{
#define ROLE(ENUM, STRING, BIT_SHIFT)      \
	if (strcmp(string, STRING) == 0) { \
		return ENUM;               \
	}
	ROLES
#undef ROLE

	assert(0 && "Unknown role");

	return ROLE_GUEST;
}

char *getRoleString(const Role role)
{
	switch (role) {
#define ROLE(ENUM, STRING, BIT_SHIFT) \
	case ENUM:                    \
		return STRING;
		ROLES
#undef ROLE
	case ROLE_GUEST:
	default:
		return "GUEST";
		break;
	}

	assert(0 && "Unknown role");

	return NULL;
}
