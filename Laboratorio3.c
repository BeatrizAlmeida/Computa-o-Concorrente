#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

long long int dim; // dimensao do vetor de entrada
float *vetor; // vetor de entrada com dimensao dim
long long int nthreads; // numero de threads
float limiarInf, limiarSup; // limiar inferior e superior

// funcao para inicializar vetor com valores aleatorios do tipo float
void inicializaVetor(){
    vetor = (float*) malloc(sizeof(float) * dim);
    if (vetor == NULL){
        fprintf(stderr, "ERRO -- malloc\n");
        exit(1);
    }
    srand(time(NULL)); // o srand vai alterar o valor o qual o rand se baseia, com base no tempo em segundos passados desde 1 de Janeiro de 1970, dessa forma o rand muda a cada execucao
    for (long long int i = 0; i < dim; i++){
        vetor[i] = rand() * 0.9; // como nao ha funcao para numeros randomicos em float em c, utilizei o rand que me retorna um inteiro aletorio e multiplico pelo 0.9
    }
}

// quantidade de valores dentro do vetor na faixa dada, resolucao de forma sequencial
long long int resolverSequencial(){
    long long int quantidade = 0;
    for (long long int i = 0; i < dim; i++){
        if (vetor[i] < limiarSup && vetor[i] > limiarInf){
            quantidade++;
        }
    }
    return quantidade;
}

// fluxo das threads
void * tarefa(void * arg){
    long long int id = (long long int) arg; // identificador da thread
    long long int tamBloco = dim / nthreads;
    long long int inicio = id * tamBloco;
    long long int fim;
    long long int *quantidade;    
    if (id == nthreads - 1){
        fim = dim;
    }else{
        fim = inicio + tamBloco;
    }

    quantidade = (long long int*) malloc(sizeof(long long int));
    if(quantidade == NULL) {
        fprintf(stderr, "ERRO -- malloc\n");
        exit(1);
    }
    *quantidade = 0;
    
    for (long long int i = inicio; i < fim; i++){
        if (vetor[i] < limiarSup && vetor[i] > limiarInf){
            (*quantidade) ++;
        }
    } 
    pthread_exit((void *) quantidade);
}

int main(int argc, char *argv[]){
    pthread_t *tid;
    long long int qtddSeq = 0, qtddConc = 0, *retorno; // quantidade de valores dentro do vetor na faixa dada sequencial e concorrente
    double inicio, fim;

    // recebe e valida os dados de entrada (dim e numero de threads)
    if (argc < 5){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads> <limiar inferior> <limiar superior>\n", argv[0]); // e como o printf, so que para arquivos, preferivel dessa forma para deixar separado as saidas de erro
        // nao e preciso abrir o arquivo stderr porque ja nos e deixado disponivel
        return 1; // similar ao exit, pois esta dentro da main
    }
    dim = atoll(argv[1]);
    nthreads = atoll(argv[2]);
    limiarInf = atof(argv[3]);
    limiarSup = atof(argv[4]);

    inicializaVetor();

    // pegar quantidade de forma sequencial
    GET_TIME(inicio);
    qtddSeq = resolverSequencial();
    GET_TIME(fim);    
    printf("Tempo sequencial: %lf\n", fim - inicio);

    // pegar quantidade de forma concorrente
    GET_TIME(inicio);
    tid  = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL){
        fprintf(stderr, "ERRO -- malloc\n");
        return 2;
    }

    // criar as threads
    for (long long int i = 0; i < nthreads; i++){
        if (pthread_create(tid+i, NULL, tarefa, (void *) i)){
            fprintf(stderr, "ERRO -- pthread_create\n");
            return 3;
        }
    }

    // aguardar o termino das threads
    for (long long int i = 0; i < nthreads; i++){
        if (pthread_join(* (tid+i), (void **) &retorno)){
            fprintf(stderr, "ERRO -- pthread_join\n");
            return 3;
        }
        qtddConc += *retorno; 
    }

    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim - inicio);

    // exibir resultados
    printf("Quantidade sequencial: %lld\n", qtddSeq);
    printf("Quantidade concorrente: %lld\n", qtddConc);

    if (qtddSeq == qtddConc){
        printf("Ambos resultaram na mesma quantidade\n");
    } else{
        printf("Erro! Valores nao sao equivalentes\n");
    }
    // libera a memoria alocada
    free(vetor);
    free(tid);
    return 0;
}
