#ifndef CLASS_H
#define CLASS_H

#include "command.h"
#include "tcp/socket.h"

#include <stdbool.h>

#define MOTD                            "Bem-vindo à turma"
#define LOGIN_SUCESS_CLIENT             "OK"
#define INVALID_USER_OR_PASSWORD_CLIENT "REJECTED"

#define CLASSES_OBJ_NAME   "/classes"
#define MULTICAST_OBJ_NAME "/multicast"

#define USERNAME_MAX_LENGTH       256
#define USER_CSV_ENTRY_MAX_LENGTH (USERNAME_MAX_LENGTH) + 256
#define CSV_DELIMITER             ";"
#define CLIENT_LOGGED_MAX         10
#define CLASS_MAX_SIZE            10
#define MAX_CLASSES               10

typedef struct LoggedClient {
	struct sockaddr_in address;
	Role role;
} LoggedClient;

#define LOGGED_CLIENT_FORMAT \
	"{\n"                \
	"  address: %s\n"    \
	"  role: %s\n"       \
	"}"

typedef struct {
	char name[USERNAME_MAX_LENGTH + 1];
	struct sockaddr_in ipMulticast;
	int maxStudents;
	int currentStudents;
	struct sockaddr_in students[CLASS_MAX_SIZE];
} Class;


bool isClientLogged(const Vector loggedClients,
                    const struct sockaddr_in clientIP);
bool isSameAddress(const struct sockaddr_in a, const struct sockaddr_in b);
struct sockaddr_in createNewIpMultiCast();
void resetClasses();
void addClientLogin(Vector *loggedClients,
                    const struct sockaddr_in clientIP,
                    const Role role);
LoggedClient *getLoggedClient(const Vector loggedClients,
                              const struct sockaddr_in clientIP);
Role getLoggedClientRole(const Vector loggedClients,
                         const struct sockaddr_in clientIP);
void setupClass(TCPSocket *tcpSocket);
void processClient(TCPSocket clientTCPSocket);
void getFormatedIp(struct sockaddr_in ip, char *buffer);

#endif // !CLASS_H
