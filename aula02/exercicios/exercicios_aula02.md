# Exercícios - Aula 02 

1. Implemente um programa utilizando a chamada de função wait() que espere todos os n processos filhos terminarem, e imprima na tela o PID e o código de retorno de cada um. O processo pai não deve guardar os PIDs dos filhos criados, e deve mostrar, no mínimo, a seguinte saída:

``` 
No processo pai: processo filho: 64464, status de saída do filho: 1
No processo pai: processo filho: 64463, status de saída do filho: 3
No processo pai: processo filho: 64462, status de saída do filho: 8
No processo pai: processo filho: 64465, status de saída do filho: 10
```

O código de retorno dos filhos deve ser o tempo que ficam em sleep; você pode utilizar um tempo aleatório para o sleep da seguinte forma:

```
#include <stdlib.h>
#include <time.h>

srand(time(NULL)+getpid());
sleep_time = rand() % 10 + 1;
sleep(sleep_time);
```

2. Substitua a chamada wait() por waitpid() no programa anterior, mantendo as demais funcionalidades.
