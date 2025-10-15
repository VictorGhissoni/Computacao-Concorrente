#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

int N,M;
int *buffer;
int qtd_no_buffer = 0;
int out = 0; //indice de retiradas
int processando = 1; // variável de controle para determinar que ainda estámos processando os números
int primos_totais = 0;
int thread_vencedora;
int qtd_primos_vencedor = 0;


//semaforos
sem_t vazio;
sem_t a_consumir;

int ehPrimo(long long int n){
    int i;
    if (n <= 1) return 0;
    if (n == 2) {/*printf("\n n eh primo, n = %d", n);*/return 1; }
    if (n%2 == 0) return 0;
    for (i = 3; i<sqrt(n) + 1; i+= 2){
        if (n%i==0) return 0;
    }
    /*printf("\n n eh primo, n = %d", n);*/
    return 1;
}

void* produtora(void* arg){
    for (int i = 0; i<= N; i++){
        sem_wait(&vazio);
        while(1){
            buffer[i%M] = i;
            qtd_no_buffer++;
            if(i==N){
                /*printf("\nbuffer = %d", buffer[0]);
                for (int j = 1; j<M; j++){
                    printf(", %d", buffer[j%M]);
                }*/
                sem_post(&a_consumir);
                break;
            }
            if (qtd_no_buffer < M){
                i++;
            }
            else {
                /*printf("\nbuffer = %d",buffer[0]);
                for (int j = 1; j<M; j++){
                    printf(", %d", buffer[j%M]);
                }*/
                sem_post(&a_consumir);
                break;
            }
        }
    }
    //printf("\nfim produtora");
    pthread_exit(NULL);
}

void* consumidora (void* arg){
    long int id = (long int) arg;
    long int local_primos = 0;
    int numero;
    long int *ret = (long int*) malloc(sizeof(void*)); 
    if(ret == NULL){
        printf("--ERRO: malloc(), retorno\n");
        exit(-1);
    }
    while (processando){
        sem_wait(&a_consumir);

        if (qtd_no_buffer > 0){
            numero = buffer[out];
            out = (out + 1) % M;
            qtd_no_buffer--;
            if (qtd_no_buffer == 0) { // fluxo de chegar no fim do buffer
                sem_post(&vazio);
            }
            else { // fluxo onde ainda tem números no buffer
                sem_post(&a_consumir);
            }
            if(ehPrimo(numero)){
                local_primos++;
            }
            if (numero == N || processando == 0){ // fluxo para a thread que chegou no último número a ser processado
               processando = 0;
               sem_post(&a_consumir);
               break; 
            }
        }
        if (processando == 0){
            sem_post(&a_consumir);
            break;
        }
    }
    //printf("\nfim consumidora %d", id);
    *ret = local_primos;
    
    pthread_exit((void*) ret);    
}

int main(int argc, char *argv[]) {
    long int* qtd_primos_retornados;
    if (argc != 4) {
        printf("\nuso './%s <N> <M> <T>'\n", argv[0]);
        printf("onde <N> eh o valor limite da checagem dos primos\n");
        printf("<M> eh o tamanho do buffer\n");
        printf("<T> eh a quantidade de threads consumidoras\n");
        exit(-1);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    int nthreads = atoi(argv[3]);
    if (M>=N){
        printf("tamanho do buffer precisa ser menor do que o número a ser checado.\n");
        exit(-1);
    }

    buffer = malloc(M * sizeof(int));

    if(buffer == NULL){
        printf("--ERRO: malloc(), buffer\n");
        exit(-1);
    }

    sem_init(&vazio, 0, 1);
    sem_init(&a_consumir, 0, 0);

    pthread_t produtor;
    if(pthread_create(&produtor, NULL, produtora, NULL)){
        printf("--ERRO: pthread_create(), produtora\n");
        exit(-1);
    }

    pthread_t *tid_consumidoras;
    tid_consumidoras = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid_consumidoras == NULL){
        printf("--ERRO: malloc() threads\n");
        exit(-1);
    }
    for (long int i = 0; i< nthreads; i++){
        if (pthread_create(&tid_consumidoras[i], NULL, consumidora, (void *)i)){
            printf("--ERRO: pthread_create(), consumidoras\n");
            exit(-1);
        }
    }

    if(pthread_join(produtor, NULL)){
        printf("--ERRO: pthread_join(), produtor\n");
        exit(-1);
    }

    for (int i = 0; i< nthreads; i++){
        if(pthread_join(tid_consumidoras[i], (void *) &qtd_primos_retornados)){
            printf("--ERRO: pthread_join(), consumidoras\n");
        }
        primos_totais += *qtd_primos_retornados;
        if(*qtd_primos_retornados > qtd_primos_vencedor){
            qtd_primos_vencedor = *qtd_primos_retornados;
            thread_vencedora = i;
        }
        printf("\nthread %d: %ld", i, *qtd_primos_retornados);
        free(qtd_primos_retornados);
    }
    printf("\n\nprimos totais: %d", primos_totais);
    printf("\nthread vencedora: %d", thread_vencedora);
    printf("\ncom %d primos\n", qtd_primos_vencedor);
    fflush(stdout);
    free(tid_consumidoras);
    free(buffer);
    sem_destroy(vazio);
    sem_destroy(a_consumir);
    return 0;
}