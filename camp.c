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

#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <limits.h>

#include "cli.h"
#include "futex.h"
#include "mem.h"

/* Número de vezes que uma thread deve entrar na região crítica. */
#define N_VEZES 3

/* Variável compartilhada */
volatile int s = 0;

pthread_t *thread_list;

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

int ultimo = 0;
int interesse[2] = {0, 0};

void *f_thread(void *v)
{
  size_t i;
  size_t thread_id = *(size_t *)v;

  for (i = 0; i < N_VEZES; i++) {
    interesse[thread_id] = 1;
    ultimo = thread_id;

    if (interesse[1 - thread_id])
      futex_wait(&ultimo, thread_id);

    s = thread_id;

    sleep(1);

    printf("Thread %d, s = %d, i = %d\n", thread_id, s, i);

    interesse[thread_id] = 0;
    futex_wake(&ultimo, INT_MAX);

    sleep(1);
  }

  return NULL;
}

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

int main(int argc, char *argv[])
{
  size_t i;
  int numthreads;

  numthreads = cli_get_thread_count(argc, argv);
  if (!numthreads) {
    cli_show_usage();
    exit(2);
  }

  thread_list = MEM_ALLOC_N(pthread_t, numthreads);

  for (i = 0; i < numthreads; i++)
    pthread_create(&thread_list[i], NULL, f_thread, &i);

  for (i = 0; i < numthreads; i++)
    pthread_join(thread_list[i], NULL);

  free(thread_list);

  return 0;
}
