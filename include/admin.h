#ifndef ADMIN_H
#define ADMIN_H

#include "command.h"
#include "udp/socket.h"

#include <netinet/in.h>
#include <stdbool.h>

#define LOGIN_SUCESS             "Logged in succesfully!"
#define INVALID_USER_OR_PASSWORD "Invalid User or Password!"
#define USER_NOT_FOUND           "User not found!"
#define USER_DELETED_SUCCESS     "User was deleted with success!"
#define ADMIN_LOGGED_MAX         10

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"

#define ADMIN_ROLE "administrator"

void setupAdminConsole(UDPSocket *serverUDPSocket);
bool isLogged(Vector *loggedAdmins, const struct sockaddr_in clientIP);
void addLogin(Vector *loggedAdmins, const struct sockaddr_in clientIP);
void printAdminIP(FILE *file, const void *const data, const size_t size);
bool socketAdressEqual(const struct sockaddr_in a, const struct sockaddr_in b);

#endif // !ADMIN_H
