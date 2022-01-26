
/* Codigo: Uma aplicacao utilizando leitores e escritores com monitor*/
class LE {
    private int leit, escr;  
    
    // Construtor
    LE() { 
       this.leit = 0; //leitores lendo (0 ou mais)
       this.escr = 0; //escritor escrevendo (0 ou 1)
    } 
    
    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
      try { 
        while (this.escr > 0) {
           System.out.println ("le.leitorBloqueado("+id+")");
           wait();  //bloqueia pela condicao logica da aplicacao 
        }
        this.leit++;  //registra que ha mais um leitor lendo
        System.out.println ("le.leitorLendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
       this.leit--; //registra que um leitor saiu
       if (this.leit == 0) 
            this.notify(); //libera escritor (caso exista escritor bloqueado)
       System.out.println ("le.leitorSaindo("+id+")");
    }
    
    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
      try { 
        while ((this.leit > 0) || (this.escr > 0)) {
           System.out.println ("le.escritorBloqueado("+id+")");
           wait();  //bloqueia pela condicao logica da aplicacao 
        }
        this.escr++; //registra que ha um escritor escrevendo
        System.out.println ("le.escritorEscrevendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
       this.escr--; //registra que o escritor saiu
       notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
       System.out.println ("le.escritorSaindo("+id+")");
    }
  }
  
  // Leitor
  class Leitor extends Thread {
    int id; //identificador da thread
    LE monitor;//objeto monitor para coordenar a lógica de execução das threads
    Inteira inteira; //variavel compartilhada
    // Construtor
    Leitor (int id, Inteira inteira, LE m) {
      this.id = id;
      this.inteira = inteira;
      this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
      try {
        
          this.monitor.EntraLeitor(this.id);
          System.out.println("A variavel e um valor " + (this.inteira.get() % 2 == 0 ? "par" : "impar"));
          this.monitor.SaiLeitor(this.id);
        
      } catch (Exception e) { return; }
    }
  }
  
  //--------------------------------------------------------
  // Escritor
  class Escritor extends Thread {
    int id; //identificador da thread
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads
    Inteira inteira; //variavel compartilhada
    // Construtor
    Escritor (int id, Inteira inteira, LE m) {
      this.id = id;
      this.inteira = inteira;
      this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
      try {

          this.monitor.EntraEscritor(this.id); 
          this.inteira.set(this.id);
          this.monitor.SaiEscritor(this.id); 
          
      } catch (Exception e) { return; }
    }
  }
  
  class LeitorEscritor extends Thread {
    int id; //identificador da thread
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads
    int delay; //atraso bobo...
    Inteira inteira; //variavel compartilhada
    // Construtor
    LeitorEscritor ( int id, int delayTime, Inteira inteira, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.inteira = inteira;
        this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
      try {
        double j=777777777.7, i;
        this.monitor.EntraLeitor(this.id);
        System.out.println("A variavel tem o valor de " + this.inteira.get());
        this.monitor.SaiLeitor(this.id);
        for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para fazer um processamento qualquer
        this.monitor.EntraEscritor(this.id); 
        this.inteira.inc(1);
        this.monitor.SaiEscritor(this.id); 
        sleep(this.delay); //atraso bobo...
          
      } catch (Exception e) { return; }
    }
 }
 
 //classe da estrutura de dados (recurso) compartilhado entre as threads
class Inteira {
    //recurso compartilhado
    private int inteira;
    //construtor
    public Inteira() { 
       this.inteira = 0; 
    }
  
    //operacao de escrita sobre o recurso compartilhado
    
    public synchronized void inc(int elemento) { 
        this.inteira += elemento; 
    }
  
    //operacao de leitura sobre o recurso compartilhado
    public int get() { 
        return this.inteira; 
    }

     //coloca em inteira o valor passado em elemento
    
     public synchronized void set(int elemento) { 
        this.inteira = elemento; 
    }
    
  }
  // Classe principal
  class Principal {
    static final int L = 4;
    static final int E = 3;
    static final int LERESC = 6;
  
    public static void main (String[] args) {
      int i;
      LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
      Leitor[] l = new Leitor[L];       // Threads leitores
      Escritor[] e = new Escritor[E];   // Threads escritores
      LeitorEscritor[] lerEsc = new LeitorEscritor[LERESC];   // Threads leitores e escritores
  
      Inteira inteira = new Inteira();
      //inicia o log de saida
      System.out.println ("import verificaLE");
      System.out.println ("le = verificaLE.LE()");
      
      for (i=0; i<E; i++) {
        e[i] = new Escritor(i+1, inteira, monitor);
        e[i].start(); 
     }
      for (i=0; i<LERESC; i++) {
        lerEsc[i] = new LeitorEscritor(i+1, (i+1)*500, inteira, monitor);
        lerEsc[i].start(); 
     }
     for (i=0; i<L; i++) {
        l[i] = new Leitor(i+1, inteira, monitor);
        l[i].start(); 
     }
    }
  }
  