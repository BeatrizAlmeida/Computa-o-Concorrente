/* Codigo: Implementar um progrma com 5 threads de forma que A thread 5 seja sempre a primeira a imprimir sua mensagem e a thread 1 seja sempre a ultima */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define NTHREADS 5
// Variaveis globais
int x = 0; //variavel compartilhada entre as threads
sem_t condt1, condtIntermediarias; //semaforos para sincronizar a ordem de execucao das threads

//funcao executada pela thread 1
void *t1 () {
    sem_wait(&condt1); //espera seu sinal para executar

    printf("Volte sempre!\n");

    pthread_exit(NULL);
}

//funcao executada pela thread 5
void *t5 () {
    printf("Seja bem-vindo!\n");

    sem_post(&condtIntermediarias); //permite que as intermediarias executem
    pthread_exit(NULL);
}

//funcao executada pela thread 2, 3 e 4
void *tIntermediarias (void *threadid) {
    int *tid = (int*) threadid;
    sem_wait(&condtIntermediarias); //espera sua condicao para executar

    switch (*tid) {
        case 2:
            printf("Fique a vontade.\n");
            break;
        case 3:
            printf("Sente-se por favor.\n");
            break;
        case 4:
            printf("Aceita um copo d'agua?\n");
            break;
        default:
            break;
    }

    x++;
    // a seguir, se x for 3 todas as intermediarias ja fizeram seu papel e t1 pode ser executada
    // no entanto, se x nao for 3, a thread libera a proxima intermediaria, a fim de que todas executem sua funcao.
    if (x == 3) {
        sem_post(&condt1);
    } else {
        sem_post(&condtIntermediarias);
    }
    pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
    pthread_t tid[NTHREADS];
    int *id[NTHREADS], t;

    for (t=0; t<NTHREADS; t++) {
        if ((id[t] = malloc(sizeof(int))) == NULL) {
            pthread_exit(NULL); 
            return 1;
        }
        *id[t] = t+1;
    }

    //inicia os semaforos
    sem_init(&condt1, 0, 0);
    sem_init(&condtIntermediarias, 0, 0);

    //cria as threads
    if (pthread_create(&tid[1], NULL, tIntermediarias, (void *)id[2])) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[0], NULL, t1, (void *)id[0])) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[2], NULL, tIntermediarias, (void *)id[1])) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[4], NULL, t5, (void *)id[2])) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[3], NULL, tIntermediarias, (void *)id[3])) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    
    //--espera todas as threads terminarem
    for (t=0; t<NTHREADS; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1);
        }
        free(id[t]);
    }
    pthread_exit(NULL);
}