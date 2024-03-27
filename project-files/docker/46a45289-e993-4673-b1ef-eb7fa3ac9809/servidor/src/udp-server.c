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
#define USERS_FILE "./include/users.txt"

char usersFile[100];
struct sockaddr_in loggedAdmins[10];

void processUdp(int portConfig, char *fileName)
{
  printf("UPD\n");

  strcpy(usersFile, fileName);

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

  printf("portConfig: %d\n", portConfig);
  // Preenchimento da socket address structure
  si_minha.sin_family = AF_INET;
  si_minha.sin_port = htons(portConfig);
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

    printf("Recebeu mensage: %s. De: %d\n", receivedBuffer, si_outra.sin_addr.s_addr);

    if (strncmp(receivedBuffer, "X", 1) == 0)
    {
      printf("Recebeu");
      continue;
    }

    // ToDo: Se for comando de login realizar login
    // (busca no ficheiro e se existir adiciona endereço no array 'loggedAdmins')
    // Se não for login: verifica se esta logado procurando na lista 'loggedAdmins'
    //    - Se não estiver retonar 'Não autrizado'
    //    - Se estiver executa 'processCommand'

    char *commandString = strtok(receivedBuffer, DELIMITER);
    printf("commandString: %s\n", commandString);

    Command command = processCommand(
        receivedBuffer,
        BUFFER_SIZE,
        responseBuffer);

    if (command == QUIT)
    {
      quit = 1;
    }

    printf("Processou comando\n");
    printf("Resposta: %s\n", responseBuffer);

    if (sendto(
            socketFd,
            responseBuffer,
            strlen(responseBuffer),
            0,
            (struct sockaddr *)&si_outra,
            slen) < 0)
    {
      error("Erro no sendto");
    }

  } while (quit != 1);

  // Fecha socket e termina programa
  // ToDo: Corrigir o close do socket
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

  char *commandString = strtok(bufferClone, "");
  char *argumentString = strtok(NULL, "");

  printf("argumentString: %s\n", argumentString);

  bool commandFound = false;
  Command command = HELP;
#define WRAPPER(enum, text, usage, function)           \
  if (strncmp(commandString, text, strlen(text)) == 0) \
  {                                                    \
    commandFound = true;                               \
    command = enum;                                    \
    function(argumentString, response);                \
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
  FILE *users;
  char *argumentClone = malloc(100);
  strncpy(argumentClone, argument, 100);
  char *argumentCloneFree = trim(argumentClone);

  char *login = strtok(argumentCloneFree, DELIMITER);
  char *password = strtok(NULL, DELIMITER);
  char *role = strtok(NULL, DELIMITER);

  char newUser[102];
  sprintf(newUser, "%s;%s;%s\n", login, password, role);

  users = fopen(usersFile, "a");
  fwrite(newUser, strlen(newUser), 1, users);

  strcpy(response, "Usuário adicionado com sucesso!\n");

  free(argumentCloneFree);
  fclose(users);
}

void deleteUser(const char *const argument, char *response)
{
  FILE *originalFile, *tempFile;
  // char buffer[256];
  // int currentLine = 1;
  char *argumentClone = malloc(100);
  strncpy(argumentClone, argument, 100);
  char *argumentFree = trim(argumentClone);

  // Open source file in read mode
  originalFile = fopen(usersFile, "r");
  if (originalFile == NULL)
  {
    printf("Erro ao abrir arquivo de usuários\n");
    strcpy(response, "Usuário não encontrado\n");
    return;
  }

  // Create temporary file in write mode
  tempFile = fopen("temp.txt", "w");
  if (tempFile == NULL)
  {
    printf("Error creating temporary file.\n");
    fclose(originalFile);
    return;
  }

  char *line = NULL;
  size_t length = 0;
  bool found = false;

  while (getline(&line, &length, originalFile) != -1)
  {
    char lineCopy[100];
    strcpy(lineCopy, line);
    char *user = strtok(lineCopy, ";");
    printf("Usuário: %s\n", user);
    printf("Argumento: %s\n", argumentFree);

    if (found || strcmp(argumentFree, user) != 0)
    {
      fputs(line, tempFile);
    }
    else
    {
      found = true;
    }
  }

  if (!found)
  {
    printf("Usuário não encontrado\n");
    strcpy(response, "Usuário não encontrado\n");
  }

  fclose(originalFile);
  fclose(tempFile);

  // Remove the original file
  remove(usersFile);

  // Rename the temporary file to the original filename
  rename("temp.txt", usersFile);
}

void listUsers(const char *const argument, char *response)
{
  FILE *users;
  char buffer[256];
  users = fopen(usersFile, "r");
  char userList[BUFFER_SIZE] = "";

  response[0] = '\0';

  if (users == NULL)
  {
    printf("Erro ao abrir arquivo de usuários\n");
    strcpy(response, "\n");
    return;
  }

  while (fgets(buffer, 256, users) != NULL)
  {
    char *user = strtok(buffer, ";");
    strcat(userList, user);
    strcat(userList, "\n");
  }

  strcpy(response, userList);

  fclose(users);
}

void quitServer(const char *const argument, char *response)
{
  (void)argument;

  strcpy(response, "Fechando servidor!\n");
}

void commandHelp(const char *const argument, char *response)
{
  printf("HELP");
}
