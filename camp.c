/*
 * Copyright 2009  Raphael Kubo da Costa <kubito@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
      thread_tree_show_interest(thread_tree, level, current_id);
      current_id = current_id / 2;

      sleep(1);
    }

    /* Critical region */
    s = thread_id;
    sleep(1);
    printf("Thread %d, s = %d\n", thread_id, s);

    /* Leave the critical region in all levels */
    for (; level > 0; level--)
      thread_tree_leave_interest(thread_tree, level - 1, thread_positions[level - 1]);

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

    if (pthread_create(&(thread_tree->thread_list[i]), NULL, f_thread, &i_vector[i]))
      fprintf(stderr, "Error while creating thread %u. Waiting for other threads to finish.\n", i);
  }

  for (i = 0; i < numthreads; i++)
    pthread_join(thread_tree->thread_list[i], NULL);

  thread_tree_free(thread_tree);
  free(i_vector);

  return 0;
}
