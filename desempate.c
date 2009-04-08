/*
 * Algoritmo do desempate proposto por Peterson.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "futex.h"

volatile int s = 0; /* Variável compartilhada */
int ultimo = 0; /* Guarda o identificador da última thread 
                        que fez acesso a esta variável */
volatile int interesse[2] = {0, 0}; /* Indica quais threads estão
                                       interessadas em entrar na região crítica */

void* f_thread_0(void *v) {
  int i;
  
  for (i = 0; i < 3; i++) {
    interesse[0] = 1;
    ultimo = 0;

    if (interesse[1])
      futex_wait(&ultimo, 0);

    s = 0;

    sleep(1);

    printf("Thread 0, s = %d, i = %d\n", s, i);

    interesse[0] = 0;
    futex_wake(&ultimo, INT_MAX);
    sleep(1);
#if 0
    interesse[0] = 1; /* Marca que esta thread está interessada */
    ultimo = 0;

    while (ultimo == 0 && interesse[1]) ;
    sleep(1);

    s = 0;
    sleep(1);
    printf("Thread 0, s = %d, i = %d.\n", s, i);
    
    interesse[0] = 0; /* Marca que saiu da região crítica */

    sleep(1);
#endif
  }

  return NULL;
}

void* f_thread_1(void *v) {
  int i;
  
  for (i = 0; i < 3; i++) {
    interesse[1] = 1;
    ultimo = 1;

    if (interesse[0])
      futex_wait(&ultimo, 1);

    s = 1;

    sleep(1);

    printf("Thread 1, s = %d, i = %d\n", s, i);

    interesse[1] = 0;
    futex_wake(&ultimo, INT_MAX);
    sleep(1);
#if 0
    interesse[1] = 1;
    ultimo = 1;
    while (ultimo == 1 && interesse[0]) ; 
    sleep(1);

    s = 1;
    sleep(1);
    printf("Thread 1, s = %d, i = %d.\n", s, i);
    
    interesse[1] = 0;
    sleep(1);
#endif
  }
  return NULL; 
}

int main() {

  pthread_t thr0, thr1;

  pthread_create(&thr0, NULL, f_thread_0, NULL);
  pthread_create(&thr1, NULL, f_thread_1, NULL);

  pthread_join(thr0, NULL); 
  pthread_join(thr1, NULL); 

  return 0;
}
