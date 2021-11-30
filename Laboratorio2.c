//multiplicacao matriz-matriz, considerando matrizes quadradas

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat1; // de entrada
float *mat2; // de entrada
float *saidaSequencial, *saidaConc; 
int nthreads;
int dim;

void * tarefa(void *arg){
    int id = (int) arg;
    for (int i = id; i < dim; i += nthreads){
        for (int j = 0; j < dim; j++){
            for (int x = 0; x < dim; x++){
                saidaConc[i*dim+j] += mat1[i*dim+x] * mat2[x*dim+j];  
            }
        }
    }
    pthread_exit(NULL);
}

void multiplicacaoSequencial(){
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            for (int x = 0; x < dim; x++){
                saidaSequencial[i*dim+j] += mat1[i*dim+x] * mat2[x*dim+j];    // logica para multiplicar a linha da primeira matriz com cada uma das colunas da segunda matriz
            }
        }
    }
}

int verificarSaidas(){
    int eIgual = 0;
    for (int i = 0; i < dim; i ++){
        for (int j = 0; j < dim; j++){
            if (saidaConc[i*dim+j] != saidaSequencial[i*dim+j]){
                eIgual = 1;
                break;
            }
        }
    }
    return eIgual;
}
int main(int argc, char* argv[]){
    double inicio, fim, deltaSeq, deltaConc;

    pthread_t *tid;
    int id;

    //leitura e avaliacao dos parametros de entrada
    if (argc < 3) printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
    /* o argv tem sempre pelo menos um argumento que é o nome da funcao*/
    dim = atoi(argv[1]); //transforma string em int
    nthreads  = atoi(argv[2]); 
    if (nthreads > dim)
        nthreads = dim;

    //alocação de memoria para as estruturas de dados
    mat1 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat1 == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    mat2 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat2 == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    saidaSequencial = (float *) malloc(sizeof(float) * dim * dim);
    if (saidaSequencial == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    saidaConc = (float *) malloc(sizeof(float) * dim * dim);
    if (saidaConc == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }

    //inicialização das estruturas de dados de entrada, utilizando a funcao rand para gerar numeros aleatorios
    for (int i = 0;i<dim; i++){
        for (int j = 0; j < dim; j++){
            mat1[i*dim+j] = rand()%100; // salva o resto da divisao por cem para os valores serem menores que 100
            mat2[i*dim+j] = rand()%100;
        }
    }

    // mutiplicacao
    // versao SEQUENCIAL
    GET_TIME(inicio);
    multiplicacaoSequencial();    
    GET_TIME(fim);
    deltaSeq = fim - inicio;
    printf("Tempo da solucao sequencial %lf\n", deltaSeq);
    
    GET_TIME(inicio);

    tid = (pthread_t*) malloc (sizeof (pthread_t) * dim);
    if(tid == NULL) {printf("erro malloc"); return 2;}

    for (int i = 0; i < nthreads; i++){
        id = i;
        if (pthread_create(tid+i, NULL, tarefa, (void*) id)){
            printf("erro no create");
            return 3;
        }
    }


    for (int i = 0; i < nthreads; i++){
        pthread_join(*(tid+i), NULL);
    }

    GET_TIME(fim);
    deltaConc = fim - inicio;
    printf("Tempo da solucao concorrente %lf\n", deltaConc);

    printf("Desempenho da solucao: %lf\n", deltaSeq/deltaConc);

    // printar saidas e matrizes opcionalmente

    // printf("Primeira matriz\n");
    // for (int i = 0; i < dim; i ++){
    //     for (int j = 0; j < dim; j++){
    //         printf("%.1f ", mat1[i*dim+j]);
    //     }
    //     printf("\n");
    // }

    // printf("Segunda matriz\n");
    // for (int i = 0; i < dim; i ++){
    //     for (int j = 0; j < dim; j++){
    //         printf("%.1f ", mat2[i*dim+j]);
    //     }
    //     printf("\n");
    // }

    // printf("Saida concorrente\n");
    // for (int i = 0; i < dim; i ++){
    //     for (int j = 0; j < dim; j++){
    //         printf("%.1f ", saidaConc[i*dim+j]);
    //     }
    //     printf("\n");
    // }

    // printf("Saida sequencial\n");
    // for (int i = 0; i < dim; i ++){
    //     for (int j = 0; j < dim; j++){
    //         printf("%.1f ", saidaSequencial[i*dim+j]);
    //     }
    //     printf("\n");
    // }

    int saoDiferentes = verificarSaidas();
    if (saoDiferentes){
        printf("As saidas sequencias e concorrentes nao sao correspondentes\n");
    }else{
        printf("As saidas estao corretas\n");
    }
    
    //liberacao da memoria
    free(mat1);
    free(mat2);
    free(saidaConc);
    free(saidaSequencial);

    return 0;
}
