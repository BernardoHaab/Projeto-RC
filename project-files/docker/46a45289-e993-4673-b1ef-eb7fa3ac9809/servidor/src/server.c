#include <unistd.h>

#include "tcp-server.h"
#include "udp-servers.h"
#include <sched.h>

int main(int argc, char **argv)
{
  pid_t wpid;
  int status = 0;

  // Cria um novo processo para lidar com o cliente
  if (fork() == 0)
  {
    processUdp();
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
