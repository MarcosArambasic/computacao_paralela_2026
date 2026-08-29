#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define TAM_MAX 100

// Função auxiliar para inverter a palavra
void inverter_palavra(char *origem, char *destino) {
    int tam = strlen(origem);
    for (int i = 0; i < tam; i++) {
        destino[i] = origem[tam - 1 - i];
    }
    destino[tam] = '\0';
}

int main() {
    int pipe1[2]; // Pai envia palavra original -> Filho lê
    int pipe2[2]; // Filho envia palavra invertida -> Pai lê
    int pipe3[2]; // Filho envia resultado palíndromo (0 ou 1) -> Pai lê

    char palavra_input[TAM_MAX];
    char palavra_invertida[TAM_MAX];
    int eh_palindromo;

    // Inicialização dos pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("Erro ao criar os pipes");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork");
        exit(EXIT_FAILURE);
    }

    // --- PROCESSO PAI ---
    if (pid > 0) {
        // Fechar pontas não utilizadas
        close(pipe1[0]); // Não lê do pipe1
        close(pipe2[1]); // Não escreve no pipe2
        close(pipe3[1]); // Não escreve no pipe3

        printf("Digite uma palavra: ");
        if (scanf("%99s", palavra_input) == EOF) {
            perror("Erro ao ler entrada");
            exit(EXIT_FAILURE);
        }

        // Envia a palavra original para o filho através do pipe1
        write(pipe1[1], palavra_input, strlen(palavra_input) + 1);
        close(pipe1[1]); // Fechar após terminar de escrever

        // Lê a palavra invertida enviada pelo filho através do pipe2
        read(pipe2[0], palavra_invertida, sizeof(palavra_invertida));
        close(pipe2[0]);

        // Lê o resultado da verificação de palíndromo através do pipe3
        read(pipe3[0], &eh_palindromo, sizeof(eh_palindromo));
        close(pipe3[0]);

        // Exibe os resultados na tela
        printf("\n[PAI] Palavra original: %s\n", palavra_input);
        printf("[PAI] Palavra invertida: %s\n", palavra_invertida);
        
        if (eh_palindromo) {
            printf("[PAI] A palavra É um palíndromo!\n");
        } else {
            printf("[PAI] A palavra NÃO é um palíndromo.\n");
        }
    } 
    // --- PROCESSO FILHO ---
    else {
        char palavra_recebida[TAM_MAX];
        char palavra_res[TAM_MAX];
        int resultado_palindromo;

        // Fechar pontas não utilizadas
        close(pipe1[1]); // Não escreve no pipe1
        close(pipe2[0]); // Não lê do pipe2
        close(pipe3[0]); // Não lê do pipe3

        // Lê a palavra original enviada pelo pai
        read(pipe1[0], palavra_recebida, sizeof(palavra_recebida));
        close(pipe1[0]);

        // Inverte a palavra recebida
        inverter_palavra(palavra_recebida, palavra_res);

        // Envia a palavra invertida para o pai através do pipe2
        write(pipe2[1], palavra_res, strlen(palavra_res) + 1);
        close(pipe2[1]);

        // Verifica se é palíndromo (comparando a original com a invertida)
        if (strcmp(palavra_recebida, palavra_res) == 0) {
            resultado_palindromo = 1; // Verdadeiro
        } else {
            resultado_palindromo = 0; // Falso
        }

        // Envia o resultado booleano/inteiro para o pai através do pipe3
        write(pipe3[1], &resultado_palindromo, sizeof(resultado_palindromo));
        close(pipe3[1]);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
