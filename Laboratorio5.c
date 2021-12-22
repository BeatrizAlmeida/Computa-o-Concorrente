/*  
    Objetivo do codigo: Implementar um progrma com 5 threads de forma que A thread 5 seja sempre a primeira a imprimir sua mensagem e a thread 1 seja sempre a ultima
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5

/* Variaveis globais */
int flagDeControle = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

/* imprime a frase “Volte sempre!”. */
void *primeira (void *t) {
    pthread_mutex_lock(&x_mutex);

    // Aqui conferimos se nossa flag ja passou por todas as threads necessarias, somente quando tiver o valor igual a Nthreads podera ser executada printando a frase final
    while (flagDeControle < NTHREADS-1) { 
        pthread_cond_wait(&x_cond, &x_mutex);
    }

    printf("Volte sempre!\n");
    pthread_mutex_unlock(&x_mutex);
 
    pthread_exit(NULL);
}

/* Thread que imprime a frase “Seja bem-vindo!”  */
void *quinta (void *t) {
    pthread_mutex_lock(&x_mutex);

    printf("Seja bem-vindo!\n");
    flagDeControle = 1; // vai atribuir um a nossa flag para identificar que ja imprimiu a primeira frase

    pthread_cond_broadcast(&x_cond); // liberando a condicao para todas as threads
    
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}

/* 
Thread que imprime as frases que devem ser printadas entre a primeira e ultima thread, variando a frase impressa de acordo com o id da thread, sendo:
A thread 2 imprime a frase “Fique a vontade.”
A thread 3 imprime a frase “Sente-se por favor.”
A thread 4 imprime a frase “Aceita um copo d’agua?.”  
*/
void *threadsIntermediarias (void *t) {
    int *my_id = (int*)t;

    pthread_mutex_lock(&x_mutex);
    // Aqui iremos conferir se nossa flag eh igual a zero para bloquear as threads, assim garantimos que ela so printara as mensagens intermediarias se ja passou pela thread que imprime a primeira frase obrigatoria
    if (flagDeControle == 0) { 
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    switch (*my_id) {
        case 2:
            printf("Fique a vontade.\n");
            break;
        case 3:
            printf("Sente-se por favor.\n");
            break;
        case 4:
            printf("Aceita um copo d'agua?\n");
            break;
    }

    flagDeControle++;
    if (flagDeControle == NTHREADS-1) {
      pthread_cond_signal(&x_cond);
    }

    pthread_mutex_unlock(&x_mutex); 

    free(my_id);
    pthread_exit(NULL);
}


/* Funcao principal */
int main(int argc, char *argv[]) {
    int *t1, *t2, *t3, *t4, *t5;
    pthread_t threads[NTHREADS];

    //aloca espaco para os identificadores das threads
    t1=malloc(sizeof(int)); 
    t2=malloc(sizeof(int)); 
    t3=malloc(sizeof(int)); 
    t4=malloc(sizeof(int));
    t5=malloc(sizeof(int));
    *t1=1, *t2=2, *t3=3, *t4=4, *t5=5; 

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);


   /* Cria as threads */
    pthread_create(&threads[0], NULL, primeira, (void *) t1); // Aqui no ultimo parametro coloco os ids das threads que estou criando
    pthread_create(&threads[1], NULL, threadsIntermediarias, (void *)t2);
    pthread_create(&threads[2], NULL, threadsIntermediarias, (void *)t3);
    pthread_create(&threads[3], NULL, threadsIntermediarias, (void *)t4);
    pthread_create(&threads[4], NULL, quinta, (void *)t5);
    
    /* Espera todas as threads completarem */
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
}
