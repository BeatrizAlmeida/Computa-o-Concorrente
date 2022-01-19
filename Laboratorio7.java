import java.util.Random;

/* Codigo: Somar elementos de um vetor de inteiros usando threads */


//classe da estrutura de dados (recurso) compartilhado entre as threads
class Somar {
    //recurso compartilhado
    private int soma;
    //construtor
    public Somar() { 
       this.soma = 0; 
    }
  
    //operacao de escrita sobre o recurso compartilhado
    
    public synchronized void inc(int elemento) { 
        this.soma += elemento; 
    }
  
    //operacao de leitura sobre o recurso compartilhado
    public int get() { 
        return this.soma; 
    }
    
  }
  

// criada uma classe que estende a classe Thread 
class SomaThread extends Thread {
    
   int[] vetor;
   int inicio, fim;
   Somar s;
   
   //--construtor
    public SomaThread(int inicio, int fim, Somar s, int[] vetor) { 
        this.fim = fim;
        this.inicio = inicio;
        this.s = s;
        this.vetor = vetor;
    }

   //--metodo executado pela thread
   public void run() {
      for (int i = this.inicio; i < this.fim; i++) {
        this.s.inc(vetor[i]);
      } 
   }

}

//--classe do metodo main
 
  
class Laboratorio7 {
    static final int N = 10;
    static final int TAM = 100;
    public static void main (String[] args) {
        //--reserva espaço para um vetor de threads
        Thread[] threads = new Thread[N];
        //cria uma instancia do recurso compartilhado entre as threads
        Somar s = new Somar();

        int[] vetor = new int[TAM];
        Random rand = new Random();
        
        // colocando valores aleatorios no vetor de inteiros
        for (int i=0; i<TAM; i++) {
            vetor[i] = rand.nextInt(1000);
        }

        int tamBloco = TAM/N;
        int posicaoInicial = 0;
        int posicaoFinal;
        // cria threads da classe que estende Thread
        for (int i=0; i<threads.length; i++) { 
            posicaoFinal = posicaoInicial + tamBloco;
            threads[i] = new SomaThread(posicaoInicial, posicaoFinal, s, vetor);
            posicaoInicial += tamBloco;
        }
  
        //iniciar as threads
        for (int i=0; i<threads.length; i++) {
           threads[i].start();
        }
  
        //esperar pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
           try { threads[i].join(); } 
           catch (InterruptedException e) { return; }
        }
        
        System.out.println("Valor da soma: " + s.get()); 

        //  Calculando a soma de forma sequencial para verificar se o valor concorrente esta correto
        int soma = 0;
        for (int elemento : vetor) {
            soma += elemento;
        }

        if (soma == s.get()) {
            System.out.println("O resultado esta correto!");
        } else {
            System.out.println("Erro, o ressultado nao esta correto!");
        }
     } 
}
