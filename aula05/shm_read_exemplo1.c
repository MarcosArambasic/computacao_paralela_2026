/* Filename: shm_read.c */
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h> // Necessário para a função sleep()

#define BUF_SIZE 1024
#define SHM_KEY 0x1234

struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};

int main(int argc, char *argv[]) {
   int shmid;
   struct shmseg *shmp;
   
   // Obtém o segmento de memória compartilhada criado pelo escritor
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644);
   if (shmid == -1) {
      perror("Shared memory get failed");
      return 1;
   }
   
   // Anexa o segmento
   shmp = shmat(shmid, NULL, 0);
   if (shmp == (void *) -1) {
      perror("Shared memory attach failed");
      return 1;
   }
   
   /* Lê os blocos de dados da memória compartilhada */
   while (shmp->complete != 1) {
      printf("Segment contains:\n\"%s\"\n", shmp->buf);
      if (shmp->cnt == -1) {
         perror("Read error");
         shmdt(shmp);
         return 1;
      }
      printf("Reading Process: Read %d bytes\n", shmp->cnt);
      sleep(3);
   }
   
   printf("Reading Process: Reading Done, Detaching Shared Memory\n");
   if (shmdt(shmp) == -1) {
      perror("Detachment failed");
      return 1;
   }
   
   printf("Reading Process: Complete\n");
   return 0;
}
