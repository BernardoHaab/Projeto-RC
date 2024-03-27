#ifndef TCP_SERVER
#define TCP_SERVER

#include <netinet/in.h>

#define SERVER_IP_DEFAULT    "0.0.0.0"
#define BUFFER_SIZE          1024
#define LISTEN_N_CONNECTIONS 5
#define MOTD                 "Bem-vindo ao servidor de nomes do DEI."
#define EXIT_MESSAGE         "Até logo!"

typedef struct {
	int fileDescriptor;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE + 1];
} TCPSocket;

struct sockaddr_in createSocketAddress(const char *const ipAddress,
                                       const int port);
TCPSocket createTCPSocket(const char *const ipAddress, const int port);

#endif // !TCP_SERVER
