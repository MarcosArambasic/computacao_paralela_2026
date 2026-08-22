#include <stdio.h>
#include <stdio.h>
#include <unistd.h>    // Necessário para fork() e sleep()
#include <stdlib.h>    // Necessário para system()
#include <sys/wait.h>  // Necessário para wait() e WEXITSTATUS

int main() {
  int pid;
  pid = fork();

  // Processo filho
  if (pid == 0) {
    system("ps -ef");
    sleep(10);
    system("ps -ef");
  } else {
    sleep(3);
  }
  return 0;
}