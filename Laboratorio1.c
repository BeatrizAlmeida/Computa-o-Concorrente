#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TAM 10000
#define NTHREADS 2

typedef struct arg_struct args;
struct arg_struct {
    int posicaoFinal, posicaoInicial;
    int * elementos;
};

void * elevaAoQuadrado (void *arg) {
    args * argumento = (args*)arg;
    int i;
    for (i = argumento->posicaoInicial; i < argumento->posicaoFinal; i++){
        argumento->elementos[i] *= argumento->elementos[i];
    }
    
    free(argumento);
    pthread_exit(NULL);
}

int main (void) {
    pthread_t tid[NTHREADS]; 
    int pos = 0, i, quadrado;
    args * arg;
    int elementos[TAM];
    //inicializando o vetor com elementos arbitrários
    for (i = 0; i < TAM; i++){
        elementos[i] = i+1;
    }

    for (i = 0; i < NTHREADS; i++){
        arg = malloc(sizeof(args));
        if (arg == NULL)
            printf("ERRO alocacao de memoria");
        //Como usarei duas threads fiz essa divisão do tamanho do array por 2 e salvei as posições iniciais e finais para não calcular nenhum elemento duas vezes
        arg->posicaoInicial = pos;
        arg->posicaoFinal = pos + TAM/2;
        arg->elementos = elementos;
        if (pthread_create(&tid[i], NULL, elevaAoQuadrado, (void *) arg))
            printf("ERRO -- pthread_create\n");
        pos = arg->posicaoFinal; 
    }

    for (i = 0; i < NTHREADS; i++){
         if (pthread_join(tid[i], NULL)){
            printf("ERRO -- pthread_join\n");
            return 1;
        }
    }

    for (i = 0; i < TAM; i++){
        quadrado = (i+1) * (i+1);
        if (elementos[i] != quadrado){
            printf("ERRO! Valor diferente do esperado no elemento %d valor atual %d valor esperado %d \n", i, elementos[i], quadrado);
            return 1;
        }
    }

    printf("Os quadrados dos elementos foram salvos com sucesso no array");
    
    return 0;
}