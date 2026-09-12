/*
EXERCÍCIO 2 - 
    Escreva um programa que use memória compartilhada para inverter uma 
    palavra fornecida pelo usuário.

NOME : MARCOS ARAMBASIC REBELO DA SILVA
RA: 10443260

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define TAM_MAX 100

// Estrutura que será armazenada na memória compartilhada
struct dados_compartilhados {
    char palavra[TAM_MAX];
    int status; // 0: Pai escreveu a palavra original, 1: Filho escreveu a palavra invertida
};

// Função auxiliar para inverter a string
void inverter_string(char *str) {
    int i = 0;
    int j = strlen(str) - 1;
    char temp;

    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int main() {
    int shmid;
    key_t chave = IPC_PRIVATE; // Cria uma chave privada para os processos relacionados
    struct dados_compartilhados *memoria;

    // 1. Cria o segmento de memória compartilhada
    shmid = shmget(chave, sizeof(struct dados_compartilhados), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Erro ao criar memória compartilhada (shmget)");
        exit(1);
    }

    // 2. Acopla a memória compartilhada ao processo pai
    memoria = (struct dados_compartilhados *) shmat(shmid, NULL, 0);
    if (memoria == (void *) -1) {
        perror("Erro ao acoplar memória (shmat)");
        exit(1);
    }

    // Inicializa o status para garantir que o filho espere
    memoria->status = 0;
    memoria->palavra[0] = '\0';

    // 3. Cria o processo filho
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork");
        exit(1);
    }

    if (pid == 0) {
        // --- CÓDIGO DO PROCESSO FILHO ---
        // O filho espera o pai escrever a palavra (aguarda status ser 0 e a string não estar vazia)
        while (memoria->status != 0 || strlen(memoria->palavra) == 0) {
            usleep(10000); // Espera de 10ms para poupar CPU
        }

        // Inverte a palavra diretamente no buffer da memória compartilhada
        inverter_string(memoria->palavra);
        
        // Altera o status para avisar o pai que a inversão está pronta
        memoria->status = 1;

        // Desacopla a memória do filho e finaliza
        shmdt(memoria);
        exit(0);

    } else {
        // --- CÓDIGO DO PROCESSO PAI ---
        printf("Digite uma palavra para inverter: ");
        fgets(memoria->palavra, TAM_MAX, stdin);

        // Remove a quebra de linha ('\n') gerada pelo fgets, se houver
        memoria->palavra[strcspn(memoria->palavra, "\n")] = '\0';

        // Aguarda o processo filho fazer a inversão (espera o status virar 1)
        while (memoria->status != 1) {
            usleep(10000);
        }

        // Exibe o resultado que o filho alterou na memória compartilhada
        printf("Palavra invertida pelo filho: %s\n", memoria->palavra);

        // Espera o processo filho encerrar completamente
        wait(NULL);

        // 4. Desacopla e destrói o segmento de memória compartilhada
        shmdt(memoria);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
