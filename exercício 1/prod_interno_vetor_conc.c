/* Disciplina: Programação Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab3:  */
/* Codigo: produto interno de 2 vetores de floats */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include "timer.h"

//#define VERSOES

//variaveis globais
//vetor de elementos
float *vet1, *vet2;
double start, finish, elapsed;

//estrutura de dados para passar argumentos para a thread
typedef struct {
   //tamanho do vetor
   long int n;
   //numero de threads 
   short int nthreads;
   //identificador da thread
   short int id;
} t_args;  


//funcao executada pelas threads
//estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *ProdInt (void *args) {
  t_args *arg = (t_args*) args; //argumentos da thread
  int ini, fim, fatia; //auxiliares para divisao do vetor em blocos
  double prod_int=0, *ret; //produto interno local
  
  fatia = arg->n / arg->nthreads; //tamanho do bloco de dados de cada thread
  ini = arg->id * fatia; //posicao inicial do vetor
  fim = ini + fatia; //posicao final do vetor
  if (arg->id == (arg->nthreads-1)) fim = arg->n; //a ultima thread trata os elementos restantes no caso de divisao nao exata

  //soma os valores 
  for(int i=ini; i<fim; i++) {
     prod_int += vet1[i]*vet2[i];
  }

  //retorna o resultado do produto interno
  ret = (double*) malloc(sizeof(double));
  if (ret!=NULL) *ret = prod_int;
  else printf("--ERRO: malloc() thread\n");
  pthread_exit((void*) ret);
}

//funcao principal do programa
int main(int argc, char *argv[]) {
  long int n; //tamanho do vetor
  short int nthreads; //numero de threads 
  FILE *arq; //arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
  double prod_ori; //produto interno registrado no arquivo
  FILE * descritorArquivo; //descritor do arquivo de saida
#ifdef VERSOES
  float soma_seq, soma_seq_blocos; //resultados das somas adicionais
  float soma1, soma2; //auxiliares para a soma sequencial alternada
#endif
  double prod_int_global; //resultado do produto interno concorrente
  double *prod_int_retorno_threads; //auxiliar para retorno das threads

  pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

  double start, finish, elapsed; // para calcular o tempo

  //valida e recebe os valores de entrada
  if(argc < 3) { printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); exit(-1); }

  //abre o arquivo de entrada com os valores para serem calculados
  arq = fopen(argv[1], "rb");
  if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

  //le o tamanho do vetor (primeira linha do arquivo)
  ret = fread(&n, sizeof(long int), 1, arq);
  if(!ret) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 3;
  }

  //aloca espaco de memoria e carrega o primeiro vetor de entrada
  vet1 = (float*) malloc (sizeof(float) * n);
  if(vet1==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
  ret = fread(vet1, sizeof(float), n, arq);
  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do primeiro vetor\n");
     return 4;
  }

  //aloca espaco de memoria e carrega o segundo vetor de entrada
  vet2 = (float*) malloc (sizeof(float) * n);
  if(vet2==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
  ret = fread(vet2, sizeof(float), n, arq);
  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do segundo vetor\n");
     return 4;
  }

  //le o numero de threads da entrada do usuario 
  nthreads = atoi(argv[2]);
  //limita o numero de threads ao tamanho do vetor
  if(nthreads>n) nthreads = n;

  //aloca espaco para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

  //cria as threads
  GET_TIME(start); //estou calcuando o tempo desde que a criação das threads até o join delas
  for(long int i=0; i<nthreads; i++) {
    t_args *args;
    args = (t_args*) malloc(sizeof(t_args));
    if(args==NULL) {    
       printf("--ERRO: malloc argumentos\n"); exit(-1);
    }
    args->n = n;
    args->nthreads = nthreads;
    args->id = i;
    if (pthread_create(&tid_sistema[i], NULL, ProdInt, (void*) args)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

#ifdef VERSOES
  //soma sequencial de traz para frente
  soma_seq = 0;
  for(int t=n-1; t>=0; t--) {
     soma_seq += vet1[t];
  }
  //soma sequencial bloco (== soma com 2 threads)
  soma1=0;
  for(int t=0; t<n/2; t++) {
     soma1 += vet1[t];
  }
  soma2=0;
  for(int t=n/2; t<n; t++) {
     soma2 += vet1[t];
  }
  soma_seq_blocos = soma1 + soma2;
#endif

  //espera todas as threads terminarem e calcula a soma total das threads
  prod_int_global=0;
  for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &prod_int_retorno_threads)) {
        printf("--ERRO: pthread_join()\n"); exit(-1);
     }
     prod_int_global += *prod_int_retorno_threads;
     free(prod_int_retorno_threads);
  }
  GET_TIME(finish); //o calculo do tempo foi feito só até o join das threads, desconsiderando o print final.

  //imprime os resultados
  char nome_arquivo[128];
  strncpy(nome_arquivo, argv[1], 38);
  strcat(nome_arquivo, " - ");
  strcat(nome_arquivo, argv[2]);
  strcat(nome_arquivo, " cores - resultados.txt");

  descritorArquivo = fopen(nome_arquivo, "w");
  if(!descritorArquivo) {
     fprintf(stderr, "Erro de abertura do arquivo\n");
     return 3;
  }

  printf("\n");
#ifdef VERSOES
  printf("soma_seq (invertida)         = %.26f\n\n", soma_seq);
  printf("soma_seq_blocos (2 blocos)   = %.26f\n\n", soma_seq_blocos);
#endif
  printf("prod_concorrente             = %.26lf\n", prod_int_global);
  fprintf(descritorArquivo, "prod_concorrente             = %.26lf\n", prod_int_global);
  
  //le o produto interno registrado no arquivo
  ret = fread(&prod_ori, sizeof(double), 1, arq); 
  printf("\nprod_ori                     = %.26lf\n", prod_ori);
  fprintf(descritorArquivo, "\nprod_ori                     = %.26lf\n", prod_ori);

  printf("\nvariacao relativa                     = %.26lf\n", (prod_ori-prod_int_global)/prod_ori);
  fprintf(descritorArquivo, "\nvariacao relativa                     = %.26lf\n", (prod_ori-prod_int_global)/prod_ori);
  elapsed = finish - start;
  printf("The code to be timed took %e seconds\n", elapsed);
  fprintf(descritorArquivo, "\nThe code to be timed took %e seconds\n", elapsed);
  //desaloca os espacos de memoria
  free(vet1);
  free(vet2);
  free(tid_sistema);
  //fecha o arquivo
  fclose(arq);
  fclose(descritorArquivo);

  return 0;
}
