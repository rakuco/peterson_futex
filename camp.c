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

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <limits.h>

#include "cli.h"
#include "futex.h"
#include "mem.h"
#include "thread_tree.h"

#define TIMES_TO_RUN 3 /**< Number of times a thread should enter the critical region */

static volatile int s = 0; /**< The shared variable that will be modified */
static ThreadTree *thread_tree; /**< The thread tree which the competing threads will use */

size_t get_other(size_t thread_id)
{
  return (thread_id % 2 ? thread_id - 1 : thread_id + 1);
}

static void enter_critical(ThreadTree *tree, size_t level, size_t thread_id, size_t real_tid)
{
  size_t other;
  size_t turn_pos;

  assert(tree);
  assert(level < tree->height);
  assert(thread_id < tree->tree[level]->n_elem);
 
  other = get_other(thread_id);
  turn_pos = thread_tree_get_turn_pos(thread_id);

  thread_tree_show_interest(tree, level, thread_id);

  if (tree->tree[level]->interested[other])
    futex_wait(&(tree->tree[level]->turn[turn_pos]), thread_id);
}

static void leave_critical(ThreadTree *tree, size_t level, size_t thread_id, size_t real_tid)
{
  size_t turn_pos;

  assert(tree);
  assert(level < tree->height);
  assert(thread_id < tree->tree[level]->n_elem);

  turn_pos = thread_tree_get_turn_pos(thread_id);

  tree->tree[level]->interested[thread_id] = 0;
  futex_wake(&(tree->tree[level]->turn[turn_pos]), 1);
}

static void *f_thread(void *v)
{
  size_t i, level;
  size_t *previous_positions;
  size_t thread_id = *(size_t *)v;
  size_t tree_height = thread_tree_get_height(thread_tree);
  size_t dyn_thread_id;
  size_t dyn_height;

  previous_positions = MEM_ALLOC_N(size_t, tree_height);

  for (i = 0; i < TIMES_TO_RUN; i++) {
    dyn_height = tree_height;
    dyn_thread_id = thread_id;

    for (level = 0; level < tree_height; level++) {
      previous_positions[level] = dyn_thread_id;
      enter_critical(thread_tree, level, dyn_thread_id, thread_id);
      dyn_thread_id = dyn_thread_id / 2;
      sleep(1);
    }

    s = thread_id;

    sleep(1);

    printf("Thread %d, s = %d\n", thread_id, s);

    for (level = level - 1; dyn_height > 0; level--, dyn_height--) {
      leave_critical(thread_tree, level, previous_positions[level], thread_id);
    }

    sleep(1);
  }

  free(previous_positions);

  return NULL;
}

int main(int argc, char *argv[])
{
  size_t i;
  size_t *i_vector;
  int numthreads;

  numthreads = cli_get_thread_count(argc, argv);
  if (!numthreads) {
    cli_show_usage();
    exit(2);
  }

  i_vector = MEM_ALLOC_N(size_t, numthreads);
  thread_tree = thread_tree_new(numthreads);

  for (i = 0; i < numthreads; i++) {
    i_vector[i] = i;
    pthread_create(&(thread_tree->thread_list[i]), NULL, f_thread, &i_vector[i]);
  }

  for (i = 0; i < numthreads; i++)
    pthread_join(thread_tree->thread_list[i], NULL);

  thread_tree_free(thread_tree);
  free(i_vector);

  return 0;
}
