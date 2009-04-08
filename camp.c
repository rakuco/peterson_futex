/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#define _GNU_SOURCE

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"
#include "futex.h"
#include "mem.h"
#include "thread_tree.h"

#define TIMES_TO_RUN 3 /**< Number of times a thread should enter the critical region */

static volatile int s = 0; /**< The shared variable that will be modified */
static ThreadTree *thread_tree; /**< The thread tree which the competing threads will use */

/**
 * Enters the critical region for a given level.
 *
 * This function marks that the given @p thread_id is interested in acessing
 * the critical region. If another thread is already in the critical region,
 * this function blocks until @p futex_wake is called.
 *
 * @param tree The ThreadTree in use.
 * @param level The level in the tree that's being accessed.
 * @param thread_id The position in the level array.
 *
 * @see futex_wake
 * @see leave_critical
 */
static void enter_critical(ThreadTree *tree, size_t level, size_t thread_id)
{
  size_t other;
  size_t turn_pos;

  assert(tree);
  assert(level < tree->height);
  assert(thread_id < tree->tree[level]->n_elem);
 
  other = (thread_id % 2 ? thread_id - 1 : thread_id + 1);
  turn_pos = thread_tree_get_turn_pos(thread_id);

  thread_tree_show_interest(tree, level, thread_id);

  if (tree->tree[level]->interested[other])
    futex_wait(&(tree->tree[level]->turn[turn_pos]), thread_id);
}

/**
 * Leaves the critical region for a given level.
 *
 * This function marks the given thread is not interested in entering the
 * critical region and wakes threads waiting to enter the critical region.
 *
 * @param tree The ThreadTree in use.
 * @param level The level in the tree that's being accessed.
 * @param thread_id The position in the level array.
 */
static void leave_critical(ThreadTree *tree, size_t level, size_t thread_id)
{
  size_t turn_pos;

  assert(tree);
  assert(level < tree->height);
  assert(thread_id < tree->tree[level]->n_elem);

  turn_pos = thread_tree_get_turn_pos(thread_id);

  tree->tree[level]->interested[thread_id] = 0;
  futex_wake(&(tree->tree[level]->turn[turn_pos]), INT_MAX);
}

/**
 * Main thread function.
 *
 * This function makes threads compete for access to the critical region.
 *
 * @param v Thread number (must be in ascending order for successive calls).
 */
static void *f_thread(void *v)
{
  size_t current_id;
  size_t i;
  size_t level;
  size_t *thread_positions;
  size_t thread_id = *(size_t *)v;
  size_t tree_height = thread_tree_get_height(thread_tree);

  thread_positions = MEM_ALLOC_N(size_t, tree_height);

  for (i = 0; i < TIMES_TO_RUN; i++) {
    current_id = thread_id;

    /* Compete for the critical region until reaching the top */
    for (level = 0; level < tree_height; level++) {
      thread_positions[level] = current_id;
      enter_critical(thread_tree, level, current_id);
      current_id = current_id / 2;

      sleep(1);
    }

    /* Critical region */
    s = thread_id;
    sleep(1);
    printf("Thread %d, s = %d\n", thread_id, s);

    /* Leave the critical region in all levels */
    for (; level > 0; level--)
      leave_critical(thread_tree, level - 1, thread_positions[level - 1]);

    sleep(1);
  }

  free(thread_positions);

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
