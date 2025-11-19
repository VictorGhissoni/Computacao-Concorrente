/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.lang.reflect.Array;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


//classe runnable
class MyCallable implements Callable<Long> {
  //construtor
  MyCallable() {}
 
  //método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i=1; i<=100; i++) {
      s++;
    }
    return s;
  }
}

class Primos implements Callable<Long> {
  private Long limite_inf;
  private Long limite_sup;
  Primos(Long lim_inf, Long lim_sup){
    this.limite_inf = lim_inf;
    this.limite_sup = lim_sup;
  }

  public boolean ehPrimo (long n){
    long i;
    if (n<=1){return false;}
    if (n==2){return true;}
    if (n%2==0){return false;}
    for (i = 3; i<Math.sqrt(n) + 1 ; i+= 2){
      if(n%i==0) {
        return false;
      }
    }
    return true;

  }
  public Long call() throws Exception {
    Long qtd_primos = 0L;
    for (long num = this.limite_inf ; num <= this.limite_sup ; num++){
      if (ehPrimo(num)) qtd_primos++;
    }
    return qtd_primos;
  }
}

//classe do método main
public class FutureHello  {
  private static final long LIMIT = 100000000;
  private static final int N = 8;
  private static final int NTHREADS = 16;

  public static void main(String[] args) {
    //cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    //cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<Long>> list = new ArrayList<Future<Long>>();

    for (int i = 0; i < N; i++) {//divisão simples entre aas threads
      long comeco = 1 + i*LIMIT/N;
      long fim = (i+1)*LIMIT/N;
      Callable<Long> worker = new Primos(comeco, fim);
      /*submit() permite enviar tarefas Callable ou Runnable e obter um objeto Future para acompanhar o progresso e recuperar o resultado da tarefa
       */
      Future<Long> submit = executor.submit(worker);
      list.add(submit);
    }

    //System.out.println("tamanho da lista" + list.size());
    //pode fazer outras tarefas...

    //recupera os resultados e faz o somatório final
    long sum = 0;
    for (Future<Long> future : list) {
      try {
        sum += future.get(); //bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println("qtd de primos ate " + LIMIT + ": " + sum);
    executor.shutdown();
  }
}
