#define _XOPEN_SOURCE 500 // Garante o suporte adequado para waitid e siginfo_t
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t pid; // Alterado para pid_t (tipo correto para PIDs)
    pid_t pids[3];
    int status;
    int num_processos = 0;
    int total_processos = 3;
    siginfo_t siginfo;

    while (num_processos < total_processos) {
        pid = fork();

        if (pid == -1) {
            perror("Erro no fork");
            return 1;
        }

        // Processo filho
        if (pid == 0) {
            printf("No processo filho: ID do processo é %d\n", getpid());
            sleep(5);
            return 4; 
        } 
        // Processo pai
        else {
            pids[num_processos] = pid;
            num_processos++;
            printf("No processo pai: processo número %d criado\n", pid);
        }
    }

    // Esperando pelo 3o processo filho (índice 2)
    // waitid retorna 0 em sucesso e -1 em erro
    status = waitid(P_PID, pids[total_processos - 1], &siginfo, WEXITED);
    
    if (status == -1) {
        perror("Erro em waitid");
        return 1;
    }

    // Exibe as informações coletadas pelo siginfo_t
    printf("\nInformações recebidas do waitid:\n");
    printf("PID do filho: %d\n", siginfo.si_pid);
    printf("ID de usuário real do filho: %d\n", siginfo.si_uid);
    printf("Código de saída do filho: %d\n", siginfo.si_status); // Mostra o "return 4" do filho

    return 0;  
}