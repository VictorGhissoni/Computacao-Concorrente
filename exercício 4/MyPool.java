/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;

//-------------------------------------------------------------------------------
//!!! Documentar essa classe !!!
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads; //criando a pool de threads
    private final LinkedList<Runnable> queue; //cria uma linked list que é a fila
    private boolean shutdown; //booleano que determina quando o trabalho acabou

    public FilaTarefas(int nThreads) { //Método para fazer a pool
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) { //Método que coloca um runnable na fila e avisa ao ser feito
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() { //Método para desligar a fila inteira
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {//Aqui tem o controle principal da fila.
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {// se a fila estiver vazia e não for hora de desligar
               try { queue.wait(); } //espera a notificação
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   //se a fila estiver vazia a gente sai do método
             r = (Runnable) queue.removeFirst(); //pegamos o primeiro elemento da fila
           }
           try { r.run(); } //e tentamos executá-lo
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
   //...completar implementacao, recebe um numero inteiro positivo e imprime se esse numero eh primo ou nao
   int num;
   public Primo(int i) { num = i; }
   private boolean ehPrimo(int n){
    int i;
    if(n<=1) {return false;}
    if(n==2) return true;
    if(n%2==0) return false;
    for(i=3; i< Math.sqrt(n)+1; i+=2) {
    if(n%i==0) return false;
     }
    return true;
    } 
   public void run() {
    if (ehPrimo(num)){
      System.out.println(num + " eh primo");
    } else {
      System.out.println(num + " nao eh primo");
    }
       //funcao para determinar se um numero  ́e primo
  }
}

//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 16;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < 50; i++) {
        final String m = "Hello da tarefa " + i;
        Runnable hello = new Hello(m);
        pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}
