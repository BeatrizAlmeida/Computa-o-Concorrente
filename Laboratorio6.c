/*
    Funcionamento da aplicacao:
    O usuario entra com o numero N de threads desejado. Em seguida, teremos N iterações, onde, a cada uma, as threads somarão os valores de todas as casas do vetor, esperarao, a partir de barreiras, todas as threads terminarem essa etapa e depois gerarao um numero aleatorio cada para colocar na casa do vetor correspondente ao seu id. Novamente, todas devem esperar o termino das N threads, 
    Por fim, as threads retornam seus acumuladores locais, que devem todos possuir o mesmo valor. Sera impresso o valor recebido de cada thread e se foram iguais ou nao.
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Variaveis globais */
int bloqueadas = 0;
int nThreads;
int *vetor;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//funcao barreira
void barreira(int nthreads) {
    pthread_mutex_lock(&x_mutex); //inicio secao critica

    if (bloqueadas == (nthreads-1)) {
        //ultima thread a chegar na barreira
        pthread_cond_broadcast(&x_cond);
        bloqueadas = 0;
    } else {
        bloqueadas ++;
        pthread_cond_wait(&x_cond, &x_mutex);
    }

    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

//funcao das threads
void *tarefa (void *arg) {
    int id = *(int*)arg;
    int *somaLocal;
    somaLocal = (int*) malloc(sizeof(int)); 
    *somaLocal = 0;
    for (int i = 0; i < nThreads; i++) {
        
        for (int j = 0; j < nThreads; j++) {
            printf("Thread %d: passo=%d soma=%d\n", id, j, *somaLocal);
            *somaLocal += vetor[j];
        }        

        //sincronizacao condicional
        barreira(nThreads);

        int randomNumber = rand() % 10;
        vetor[id] = randomNumber;

        //sincronizacao condicional
        barreira(nThreads);
    }

    pthread_exit((void *) somaLocal);
}

// Funcao principal 
int main(int argc, char *argv[]) {
    int *vetorResultados;
    int flag = 1; 

    //leitura e avaliacao dos parametros de entrada
    if (argc < 2) printf("Digite: %s <numero de threads>\n", argv[0]);
    /* o argv tem sempre pelo menos um argumento que é o nome da funcao*/

    nThreads  = atoi(argv[1]); 
    
    pthread_t threads[nThreads];
    int id[nThreads], numeroRandom, *retorno;
    // inicializa o vetor com numeros aleatorios entre 0 e 9
    vetor = (int*) malloc( nThreads * sizeof(int));
    for (int i = 0; i < nThreads; i++) {    
        numeroRandom = rand() % 10;
        vetor[i] = numeroRandom;
    }

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de
    condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);

    /* Cria as threads */
    for(int i = 0;i < nThreads; i++) {
        id[i] = i;
        pthread_create(&threads[i], NULL, tarefa, (void *) &id[i]);
    }

    /* Espera todas as threads completarem */
    vetorResultados = (int*) malloc( nThreads * sizeof(int));
    for (int i = 0; i < nThreads; i++) {
        pthread_join(threads[i], (void **) &retorno);
        vetorResultados[i] = *retorno;
        if (i != 0) {
            if (vetorResultados[i] != vetorResultados[i-1])
                flag = 0;
        }
        printf("Valor da thread %d: %d\n", i+1, *retorno);
    }

    if (flag) {
        printf("Todos os valores recebidos sao iguais!");
    } else {
        printf("Erro! Nem todos valores sao iguais");
    }

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    return 0;
}