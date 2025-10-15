#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int N;

int ehPrimo(long long int n){
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n%2 == 0) return 0;
    for (i = 3; i<sqrt(n) + 1; i+= 2){
        if (n%i==0) return 0;
    }
    //printf("\n n eh primo, n = %d", n);
    return 1;
}

int main (int argc, char *argv[]) {
    int qtdprimos = 0;
    if (argc != 2){
        printf("\n--ERRO: uso ./%s <N>", argv[0]);
        printf("\nonde <N> é o limite dos primos a checar.");
        exit(-1);
    }
    N = atoi(argv[1]);
    for (int i = 0; i<=N; i++){
        if(ehPrimo(i)){
            qtdprimos ++;
        }
    }
    printf("\n\nprimos totais: %d", qtdprimos);
    fflush(stdout);
    return 0;

}