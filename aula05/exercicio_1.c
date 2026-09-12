/*
EXERCÍCIO 1 - 
    Escreva um programa que use memória compartilhada para
    verificar se uma palavra fornecida pelo usuário é palíndrome ou não.

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
#include <ctype.h>

#define TAM_MAX 100

// Estrutura que será armazenada na memória compartilhada
struct dados_compartilhados {
    char palavra[TAM_MAX];
    int status;        // 0: Pai escreveu, 1: Filho processou
    int eh_palindromo; // 0: Não é, 1: É palíndromo
};

// Função auxiliar para verificar palíndromo
int verificar_palindromo(char *str) {
    int inicio = 0;
    int fim = strlen(str) - 1;

    while (fim > inicio) {
        if (tolower(str[inicio]) != tolower(str[fim])) {
            return 0; // Não é palíndromo
        }
        inicio++;
        fim--;
    }
    return 1; // É palíndromo
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

    // Inicializa o status
    memoria->status = 0;

    // 3. Cria o processo filho
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork");
        exit(1);
    }

    if (pid == 0) {
        // --- CÓDIGO DO PROCESSO FILHO ---
        // O filho espera o pai escrever a palavra (status mudar para 0 e palavra não estar vazia)
        while (memoria->status != 0 || strlen(memoria->palavra) == 0) {
            usleep(10000); // Espera curta de 10ms para não travar a CPU
        }

        // Remove a quebra de linha se houver
        memoria->palavra[strcspn(memoria->palavra, "\n")] = '\0';

        // Verifica se é palíndromo e salva o resultado na memória
        memoria->eh_palindromo = verificar_palindromo(memoria->palavra);
        
        // Altera o status para avisar o pai que terminou
        memoria->status = 1;

        // Desacopla a memória do filho e finaliza
        shmdt(memoria);
        exit(0);

    } else {
        // --- CÓDIGO DO PROCESSO PAI ---
        printf("Digite uma palavra: ");
        fgets(memoria->palavra, TAM_MAX, stdin);

        // Aguarda o processo filho terminar a validação
        while (memoria->status != 1) {
            usleep(10000);
        }

        // Exibe o resultado final com base no que o filho escreveu
        if (memoria->eh_palindromo) {
            printf("Resultado: A palavra '%s' É um palíndromo!\n", memoria->palavra);
        } else {
            printf("Resultado: A palavra '%s' NÃO é um palíndromo.\n", memoria->palavra);
        }

        // Espera o processo filho encerrar completamente
        wait(NULL);

        // 4. Desacopla e destrói a memória compartilhada
        shmdt(memoria);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
