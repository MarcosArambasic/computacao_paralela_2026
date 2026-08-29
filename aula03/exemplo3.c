#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
   int pipefds1[2], pipefds2[2];
   int returnstatus1, returnstatus2;
   int pid;
   char pipe1writemessage[20] = "Hi";
   char pipe2writemessage[20] = "Hello";
   char readmessage[20];

   // Cria o Pipe 1
   returnstatus1 = pipe(pipefds1);
   if (returnstatus1 == -1) {
      printf("Unable to create pipe 1 \n");
      return 1;
   }

   // Cria o Pipe 2
   returnstatus2 = pipe(pipefds2);
   if (returnstatus2 == -1) {
      printf("Unable to create pipe 2 \n");
      return 1;
   }

   // Cria o processo filho
   pid = fork();

   if (pid < 0) {
      printf("Falha ao executar o fork\n");
      return 1;
   }

   if (pid == 0) {
      // --- PROCESSO FILHO ---
      // Filho lê do Pipe 1 (fecha a escrita do Pipe 1)
      close(pipefds1[1]);
      // Filho escreve no Pipe 2 (fecha a leitura do Pipe 2)
      close(pipefds2[0]);

      // 1. Filho lê a mensagem do Pai (Pipe 1)
      read(pipefds1[0], readmessage, sizeof(readmessage));
      printf("Filho leu do Pipe 1: %s\n", readmessage);

      // 2. Filho envia uma resposta para o Pai (Pipe 2)
      printf("Filho escrevendo no Pipe 2: %s\n", pipe2writemessage);
      write(pipefds2[1], pipe2writemessage, sizeof(pipe2writemessage));

      // Fecha o que restou
      close(pipefds1[0]);
      close(pipefds2[1]);
   } else {
      // --- PROCESSO PAI ---
      // Pai escreve no Pipe 1 (fecha a leitura do Pipe 1)
      close(pipefds1[0]);
      // Pai lê do Pipe 2 (fecha a escrita do Pipe 2)
      close(pipefds2[1]);

      // 1. Pai envia a mensagem para o Filho (Pipe 1)
      printf("Pai escrevendo no Pipe 1: %s\n", pipe1writemessage);
      write(pipefds1[1], pipe1writemessage, sizeof(pipe1writemessage));

      // 2. Pai lê a resposta do Filho (Pipe 2)
      read(pipefds2[0], readmessage, sizeof(readmessage));
      printf("Pai leu do Pipe 2: %s\n", readmessage);

      // Fecha o que restou
      close(pipefds1[1]);
      close(pipefds2[0]);

      // Espera o filho terminar
      wait(NULL);
   }

   return 0;
}
