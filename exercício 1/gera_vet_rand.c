/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
//#define LOG 

int main(int argc, char*argv[]) {
   float *vetor; //vetor 1 que será gerado
   float *vetor2; //vetor 2 que será gerado
   long int n; //qtde de elementos do vetor
   float elem; //valor gerado para incluir no vetor
   double prod_int=0; //valor do produto interno a ser armazenado
   int fator=1; //fator multiplicador para gerar números negativos
   FILE * descritorArquivo; //descritor do arquivo de saida
   FILE * descritorArquivo2;
   size_t ret; //retorno da funcao de escrita no arquivo de saida

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para os vetores
   vetor = (float*) malloc(sizeof(float) * n);
   vetor2 = (float*) malloc(sizeof(float) * n);
   if(!vetor) {
      fprintf(stderr, "Erro de alocação da memoria do vetor 1\n");
      return 2;
   }
   if(!vetor2) {
      fprintf(stderr, "Erro de alocação da memória do vetor 2\n");
      return 2;
   }

   //preenche o vetor com valores float aleatorios
   srand(time(NULL));
   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor[i] = elem;
        fator = (rand() % 2 == 0) ? 1 : -1; 
   }
   for(long int i=0; i<n; i++){
        elem = (rand() % MAX)/3.0 * fator;
        vetor2[i] = elem;
        fator = (rand() % 2 == 0) ? 1 : -1; 
        prod_int += vetor[i]*vetor2[i];      
   }

   //imprimir na saida padrao o vetor gerado
   #ifdef LOG
   fprintf(stdout, "dimensao: %ld\n vetor 1:\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor[i]);
   }
   fprintf(stdout, "\nvetor 2:\n");
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor2[i]);
   }
   fprintf(stdout, "\nproduto interno: %lf", prod_int);
   #endif

   //abre o arquivo para escrita binaria

   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve a dimensao
   ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   //escreve os elementos do 1 vetor
   ret = fwrite(vetor, sizeof(float), n, descritorArquivo);
   if(ret < n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   //escreve os elementos do 2 vetor
   ret = fwrite(vetor2, sizeof(float), n, descritorArquivo);
   if(ret < n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   //escreve o produto interno dos vetores
   ret = fwrite(&prod_int, sizeof(double), 1, descritorArquivo);

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor);
   return 0;
} 
