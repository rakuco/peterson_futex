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
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include "futex.h"
#include "mem.h"
#include "thread_tree.h"

#ifdef DEBUG
#include <stdio.h>
#endif

void thread_level_free(ThreadLevel *level)
{
  if (level) {
    free(level->interested);
    free(level->turn);
    free(level);
  }
}

size_t thread_level_get_turn_pos(size_t thread_id)
{
  return thread_id/2;
}

ThreadLevel *thread_level_new(size_t numthreads)
{
  ThreadLevel *level;

  level = MEM_ALLOC(ThreadLevel);

  /* Always pad numthread to the next even number */
  level->interested = MEM_ALLOC_N(size_t, (numthreads % 2 ? numthreads + 1 : numthreads));
  level->n_elem = (numthreads % 2 ? numthreads + 1 : numthreads);
  level->turn = MEM_ALLOC_N(size_t, level->n_elem / 2);

  return level;
}

void thread_tree_enter_critical_region(ThreadTree *tree, size_t level, size_t thread_id, size_t real_tid)
{
  size_t other, turn_pos;

  assert(tree);
  assert(level < thread_tree_get_height(tree));
  assert(thread_id < tree->tree[level]->n_elem);

  other = (thread_id % 2 ? thread_id - 1 : thread_id + 1);
  turn_pos = thread_level_get_turn_pos(thread_id);

  tree->tree[level]->interested[thread_id] = 1;
  tree->tree[level]->turn[turn_pos] = thread_id;

#ifdef DEBUG
  fprintf(stderr, "enter_critical: level = %u, tid = %u, interest = %u, other interest = %u\n",
           level, real_tid, this->tree[level]->interested[thread_id], this->tree[level]->interested[other]);
#endif

  /* First wake any sleeping thread.
   * Only then try to put the current thread to sleep. The current thread
   * may not be put to sleep at all too if it was the other thread's turn. */
  if ((tree->tree[level]->interested[other])) {
    if ((futex_wake(&(tree->tree[level]->turn[turn_pos]), 1)) && (tree->tree[level]->turn[turn_pos] == other))
      futex_wait(&(tree->tree[level]->turn[turn_pos]), other);
    else
      futex_wait(&(tree->tree[level]->turn[turn_pos]), thread_id);
  }
}

void thread_tree_free(ThreadTree *tree)
{
  size_t i;

  if (tree) {
    for (i = 0; i < tree->height; i++)
      thread_level_free(tree->tree[i]);

    free(tree->thread_list);
    free(tree->tree);
    free(tree);
  }
}

size_t thread_tree_get_height(ThreadTree *tree)
{
  assert(tree);

  return tree->height;
}

void thread_tree_leave_critical_region(ThreadTree *tree, size_t level, size_t thread_id, size_t real_tid)
{
  size_t turn_pos;

  assert(tree);
  assert(level < thread_tree_get_height(tree));
  assert(thread_id < tree->tree[level]->n_elem);

  turn_pos = thread_level_get_turn_pos(thread_id);

#ifdef DEBUG
  fprintf(stderr, "leave_critical: level = %u, tid = %u\n", level, real_tid);
#endif

  tree->tree[level]->interested[thread_id] = 0;
  futex_wake(&(tree->tree[level]->turn[turn_pos]), 1);
}

ThreadTree *thread_tree_new(size_t numthreads)
{
  size_t height = 0;
  size_t threads_left;
  ThreadLevel *level;
  ThreadTree *tree;

  assert(numthreads > 0);

  tree = MEM_ALLOC(ThreadTree);
  tree->tree = NULL;
  tree->thread_list = MEM_ALLOC_N(pthread_t, numthreads);

  for (threads_left = numthreads; threads_left > 0; threads_left /= 2) {
    level = thread_level_new(numthreads);

    tree->tree = realloc(tree->tree, (height * sizeof(tree->tree)) + sizeof(ThreadLevel*));
    tree->tree[height] = level;

    height++;
  }

  tree->height = height;

  return tree;
}
