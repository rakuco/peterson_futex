/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
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
    free(level->list);
    free(level);
  }
}

ThreadLevel *thread_level_new(size_t numthreads)
{
  ThreadLevel *level;

  level = MEM_ALLOC(ThreadLevel);

  level->interested = MEM_ALLOC_N(size_t, (numthreads % 2 ? numthreads + 1 : numthreads));
  level->list = MEM_ALLOC_N(pthread_t, numthreads);
  level->n_elem = numthreads;
  level->turn = 0;

  return level;
}

void thread_tree_free(ThreadTree *tree)
{
  size_t i;

  if (tree) {
    for (i = 0; i < tree->height; i++)
      thread_level_free(tree->tree[i]);

    free(tree->tree);
    free(tree);
  }
}

ThreadTree *thread_tree_new(size_t numthreads)
{
  size_t height = 0;
  ThreadLevel *level;
  ThreadTree *tree;

  assert(numthreads > 0);

  tree = MEM_ALLOC(ThreadTree);
  tree->tree = NULL;

  while (numthreads) {
    level = thread_level_new(numthreads);

    tree->tree = realloc(tree->tree, sizeof(tree->tree) + sizeof(ThreadLevel*));
    tree->tree[height] = level;

    ++height;
    numthreads = numthreads / 2;
  }

  tree->height = height;

  return tree;
}
