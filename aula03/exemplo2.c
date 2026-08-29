#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
   int pipefds[2];
   int returnstatus;
   int pid;
   char writemessages[2][20] = {"Hi", "Hello"};
   char readmessage[20];

   // Cria o pipe
   returnstatus = pipe(pipefds);
   if (returnstatus == -1) {
      printf("Não foi possível criar o pipe\n");
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
      // O filho vai ler do pipe, então fechamos a saída (escrita)
      close(pipefds[1]);

      // Lê a primeira mensagem
      read(pipefds[0], readmessage, sizeof(readmessage));
      printf("Filho leu: %s\n", readmessage);

      // Lê a segunda mensagem
      read(pipefds[0], readmessage, sizeof(readmessage));
      printf("Filho leu: %s\n", readmessage);

      // Fecha a entrada após ler
      close(pipefds[0]);
   } else {
      // --- PROCESSO PAI ---
      // O pai vai escrever no pipe, então fechamos a entrada (leitura)
      close(pipefds[0]);

      printf("Pai enviando mensagens...\n");
      
      // Escreve as duas mensagens no pipe
      write(pipefds[1], writemessages[0], sizeof(writemessages[0]));
      write(pipefds[1], writemessages[1], sizeof(writemessages[1]));

      // Fecha a saída após escrever
      close(pipefds[1]);

      // Espera o filho terminar antes de fechar o programa
      wait(NULL);
   }

   return 0;
}
