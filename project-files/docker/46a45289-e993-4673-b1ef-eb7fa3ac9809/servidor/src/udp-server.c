#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#include "udp-servers.h"
#include "udp-command.h"
#include "utils.h"

#define SERVER_IP_DEFAULT "0.0.0.0"
#define SERVER_PORT_DEFAULT 8080
#define BUFFER_SIZE 512 // Tamanho do buffer

#define DELIMITER " "

void processUdp()
{
  printf("UPD\n");

  struct sockaddr_in si_minha, si_outra;
  int socketFd, receivedBytes;
  char receivedBuffer[BUFFER_SIZE], responseBuffer[BUFFER_SIZE];
  socklen_t slen = sizeof(si_outra);

  printf("Vai criar socket\n");

  // Cria um socket para recepção de pacotes UDP
  if ((socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    error("Erro na criação do socket");
  }

  // Preenchimento da socket address structure
  si_minha.sin_family = AF_INET;
  si_minha.sin_port = htons(SERVER_PORT_DEFAULT);
  si_minha.sin_addr.s_addr = inet_addr(SERVER_IP_DEFAULT);

  printf("Vai associar porta\n");

  // Associa o socket à informação de endereço
  if (bind(socketFd, (struct sockaddr *)&si_minha, sizeof(si_minha)) == -1)
  {
    error("Error binding socket\n");
  }

  printf("UDP ESPERA MESSAGE\n");
  int quit = 0;

  do
  {
    // Espera recepção de mensagem (a chamada é bloqueante)
    if ((receivedBytes = recvfrom(
             socketFd,
             receivedBuffer,
             BUFFER_SIZE,
             0,
             (struct sockaddr *)&si_outra,
             (socklen_t *)&slen)) == -1)
    {
      error("Erro no recvfrom");
    }

    // Para ignorar o restante conteúdo (anterior do buffer)
    receivedBuffer[receivedBytes] = '\0';

    printf("Recebeu mensage: %s", receivedBuffer);

    Command command = processCommand(
        receivedBuffer,
        BUFFER_SIZE,
        responseBuffer);

    printf("Processou comando");

  } while (quit != 1);

  // Fecha socket e termina programa
  close(socketFd);
}

Command processCommand(
    const char *const buffer,
    const size_t bufferSize,
    char *response)
{
  char *bufferClone = malloc(sizeof(buffer[0]) * bufferSize);

  strncpy(bufferClone, buffer, bufferSize);

  char *bufferCloneFree = trim(bufferClone);

  char *commandString = strtok(bufferClone, DELIMITER);

  bool commandFound = false;
  Command command = HELP;
#define WRAPPER(enum, text, usage, function)           \
  if (strncmp(commandString, text, strlen(text)) == 0) \
  {                                                    \
    commandFound = true;                               \
    command = enum;                                    \
    function(bufferClone, response);                   \
  }
  COMMAND_ENUM
#undef WRAPPER

  if (!commandFound)
  {
    strcpy(response, "Command not found. Tip: Type \"help\"\n");
  }

  free(bufferCloneFree);

  return command;
}

void addUser(const char *const argument, char *response)
{
  printf("ADD_USER");
}

void commandHelp(const char *const argument, char *response)
{
  printf("HELP");
}
