#include <unistd.h>
#include <stdio.h>

#include "tcp-server.h"
#include "udp-servers.h"
#include <sched.h>

int main(int argc, char *argv[])
{
  pid_t wpid;
  int status = 0;
  int portTurmas, portConfig;
  char *ficheiroConfig;

  if (argc != 4)
  {
    printf("server <port_turmas> <port_config> <ficheiro_config>\n");
    exit(-1);
  }

  portTurmas = atoi(argv[1]);
  portConfig = atoi(argv[2]);
  ficheiroConfig = argv[3];

  printf("portTurmas: %d\n", portTurmas);
  printf("portConfig: %d\n", portConfig);
  printf("ficheiroConfig: %s\n", ficheiroConfig);

  // Cria um novo processo para lidar com o cliente
  if (fork() == 0)
  {
    processUdp(portConfig, ficheiroConfig);
    exit(0);
    // close(serverSocketFD);
    // sendMOTD(clientSocketFD, MOTD "\n");
    // processClient(clientSocketFD, clientIPAddress);
    // exit(0);
  }

  if (fork() == 0)
  {
    processTcp();
    exit(0);
  }

  while ((wpid = wait(&status)) > 0)
    ; // this way, the father waits for all the child
}
