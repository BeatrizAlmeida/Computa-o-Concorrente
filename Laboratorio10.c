/* Codigo: a cada  execução de um produtor, ele preenche o buffer inteiro, e não apenas um  único item (para isso ele deve esperar o buffer ficar completamente vazio). O consumidor continua com a mesma logica, i.e., consome um item de cada vez.*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define TAMBUFFER 5

// Variaveis globais
int leitores = 0, chegaram; 
sem_t slotVazio, slotCheio, mutexCons, mutexProd; 
int Buffer[TAMBUFFER];

int consumidor() {
    int item;
    static int out=0;
    printf("Thread consumidora -> entrou\n");
    //aguarda slot cheio
    sem_wait(&slotCheio);
    sem_wait(&mutexCons);

    item = Buffer[out];
    out = (out + 1) % TAMBUFFER;

    printf("Thread -> consumiu\n");

    //sinaliza um slot vazio
    sem_post(&mutexCons);
    sem_post(&slotVazio);

    return item;
}

void produtor(int item) {
    printf("Thread produtora -> entrou\n");
    //aguarda slot vazio
    sem_wait(&slotVazio);
    //exclusao mutua entre produtores
    sem_wait(&mutexProd);
    
    for (int i = 0; i < TAMBUFFER; i++) {
        Buffer[i] = item;
    }

    printf("Thread -> produziu\n");
    sem_post(&mutexProd);
    sem_post(&slotCheio);
}

int main(int argc, char const *argv[]) {
    int nConsumidoras, nProdutoras;

    //leitura e avaliacao dos parametros de entrada
    if (argc < 3) printf("Digite: %s <numero de threads consumidoras>  <numero de threads produtoras>\n", argv[0]);
    /* o argv tem sempre pelo menos um argumento que é o nome da funcao*/

    nConsumidoras  = atoi(argv[1]); 
    nProdutoras = atoi(argv[2]); 

    //inicia os semaforos
    sem_init(&slotCheio, 0, 0);
    sem_init(&slotVazio, 0, TAMBUFFER);
    sem_init(&mutexCons, 0, 1);
    sem_init(&mutexProd, 0, 1);


    pthread_t threadsCons[nConsumidoras], threadsProd[nProdutoras];
    int t;


    /* Cria as threads */
    for(int i = 0;i < nConsumidoras; i++) {
        pthread_create(&threadsCons[i], NULL, consumidor, rand() % 10);
    }

    for(int i = 0;i < nProdutoras; i++) {
        pthread_create(&threadsProd[i], NULL, produtor, rand() % 10); // passando o item como um numero randomico
    }
    
    //--espera todas as threads terminarem
    for (t=0; t<nConsumidoras; t++) {
        if (pthread_join(threadsCons[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1);
        }
    }
    for (t=0; t<nProdutoras; t++) {
        if (pthread_join(threadsProd[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1);
        }
    }

    pthread_exit(NULL);
    return 0;
}
