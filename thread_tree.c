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
#include "mem.h"
#include "thread_tree.h"

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

void thread_tree_show_interest(ThreadTree *tree, size_t level, size_t thread_id)
{
  size_t turn_pos;

  assert(tree);
  assert(level < thread_tree_get_height(tree));

  turn_pos = thread_level_get_turn_pos(thread_id);

  tree->tree[level]->interested[thread_id] = 1;
  tree->tree[level]->turn[turn_pos] = thread_id;
}
