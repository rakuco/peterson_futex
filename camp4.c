/* 
 * Campeonato entre 4 threads para acesso à região crítica.
 *
 * Modifique este código para funcionar para N threads,
 * com apenas uma função f_thread.
 *
 * Deve ser colocado um comando sleep nos seguintes pontos:
 *   - entre as partidas;
 *   - entre a atribuição à variável s e a impressão do
 *     valor desta variável;
 *   - fora da região crítica.  
 *
 * Substitua as esperas ocupadas por chamadas a futex_wait. 
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/futex.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <limits.h>

/* Retorna -1 se o futex não bloqueou e 
            0 caso contrário */
int futex_wait(void *addr, int val1) {
  return syscall(SYS_futex, addr, FUTEX_WAIT, 
                 val1, NULL, NULL, 0);
}

/* Retorna o número de threads que foram acordadas */
int futex_wake(void *addr, int n) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, 
                   n, NULL, NULL, 0);
}

/* Número de vezes que uma thread deve entrar na região crítica. */
#define N_VEZES 15

/* Variável compartilhada */
volatile int s = 0;

/* Variáveis de controle para a final. */
int ultimo_final = 0;
int interesse_final[2] = { 0, 0 };

/* Variáeis de controle para a partida
   entre a thread 0 e a thread 1.      */
int ultimo_01 = 0;
int interesse_01[2] = { 0, 0 };

/* Variáveis de controle para a partida
   entre a thread 2 e a thread 3.      */
int ultimo_23 = 0;
int interesse_23[2] = { 0, 0 };

/* Função para a thread 0 */
void* f_thread_0(void *v) {
  int i;
  
  for (i = 0; i < N_VEZES; i++) {

    interesse_01[0] = 1;
    ultimo_01 = 0;
    while (ultimo_01 == 0 && interesse_01[1]) ; 

    sleep(1); /* Sleep entre as partidas */
     
    interesse_final[0] = 1;
    ultimo_final = 0;
    
    while (ultimo_final == 0 && interesse_final[1]) ; 

    s = 0;
    sleep(1); /* Sleep entre a atribuição e a impressão */    
    printf("Thread 0, s = %d.\n", s); 

    interesse_final[0] = 0;    
    interesse_01[0] = 0;

    sleep(1); /* Sleep fora da região crítica */
  }

  return NULL;
}

/* Função para a thread 1 */
void* f_thread_1(void *v) {
  int i;
  
  for (i = 0; i < N_VEZES; i++) {

    interesse_01[1] = 1;
    ultimo_01 = 1;
    while (ultimo_01 == 1 && interesse_01[0]) ; 

    sleep(1); /* Sleep entre as partidas */
    
    interesse_final[0] = 1;
    ultimo_final = 1;
    
    while (ultimo_final == 1 && interesse_final[1]) ; 

    s = 1;
    sleep(1); /* Sleep entre a atribuição e a impressão */     
    printf("Thread 1, s = %d.\n", s); 

    interesse_final[0] = 0;    
    interesse_01[1] = 0;

    sleep(1); /* Sleep fora da região crítica */    
  }

  return NULL;
}

/* Função para a thread 2 */
void* f_thread_2(void *v) {
  int i;
  
  for (i = 0; i < N_VEZES; i++) {

    interesse_23[0] = 1;
    ultimo_23 = 2;
    while (ultimo_23 == 2 && interesse_23[1]) ; 

    sleep(1); /* Sleep entre as partidas */
    
    interesse_final[1] = 1;
    ultimo_final = 2;
    while (ultimo_final == 2 && interesse_final[0]) ; 

    s = 2;
    sleep(1); /* Sleep entre a atribuição e a impressão */        
    printf("Thread 2, s = %d.\n", s); 

    interesse_final[1] = 0;    
    interesse_23[0] = 0;

    sleep(1); /* Sleep fora da região crítica */    
  }

  return NULL;
}

/* Função para a thread 3 */
void* f_thread_3(void *v) {
  int i;
  
  for (i = 0; i < N_VEZES; i++) {

    interesse_23[1] = 1;
    ultimo_23 = 3;
    while (ultimo_23 == 3 && interesse_23[0]) ; 

    sleep(1); /* Sleep entre as partidas */
    
    interesse_final[1] = 1;
    ultimo_final = 3;
    while (ultimo_final == 3 && interesse_final[0]) ; 

    s = 3;
    sleep(1);    
    printf("Thread 3, s = %d.\n", s); 

    interesse_final[1] = 0;    
    interesse_23[1] = 0;

    sleep(1); /* Sleep fora da região crítica */    
  }

  return NULL;
}

int main() {

  pthread_t thr0, thr1, thr2, thr3;

  pthread_create(&thr0, NULL, f_thread_0, NULL);
  pthread_create(&thr1, NULL, f_thread_1, NULL);
  pthread_create(&thr2, NULL, f_thread_2, NULL);
  pthread_create(&thr3, NULL, f_thread_3, NULL);

  pthread_join(thr0, NULL); 
  pthread_join(thr1, NULL); 
  pthread_join(thr2, NULL); 
  pthread_join(thr3, NULL); 

  return 0;
}

