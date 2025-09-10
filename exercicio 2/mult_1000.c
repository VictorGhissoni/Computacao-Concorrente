/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <stdbool.h>

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond; //variavel de condição 1
pthread_cond_t cond_print; //variavel de condicão 2
bool hora_de_printar = false;  //variável global que controla o estado

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     while(hora_de_printar==true){
        pthread_cond_wait(&cond, &mutex);
     }
     //--SC (seção critica)
     soma++; //incrementa a variavel compartilhada 

     if(soma%1000 == 0){
        pthread_cond_signal(&cond_print);
        hora_de_printar = true;
        pthread_cond_wait(&cond, &mutex);
     }
     //--saida da SC
     pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  long int nthreads = (long int) args;
  printf("Extra : esta executando...\n");
  long int count = 0;

  while (count < nthreads*100){
    pthread_mutex_lock(&mutex);
    while (hora_de_printar==false){
        pthread_cond_wait(&cond_print, &mutex);
    }

    printf("soma = %ld \n", soma);
    count++;

    pthread_cond_broadcast(&cond);
    hora_de_printar = false;

    pthread_mutex_unlock(&mutex);
  }
  //printf("soma = %ld \n", soma);
  //pthread_cond_broadcast(&cond);
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   
   pthread_cond_init (&cond, NULL);
   pthread_cond_init (&cond_print, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, (void *) nthreads)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);
   pthread_cond_destroy(&cond_print);
   
   //printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
