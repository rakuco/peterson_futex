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

ThreadTree *thread_tree_new(size_t numthreads)
{
  size_t height = 0;
  ThreadLevel *level;
  ThreadTree *tree;

  assert(numthreads > 0);

  tree = MEM_ALLOC(ThreadTree);
  tree->tree = NULL;

  while (numthreads) {
    level = MEM_ALLOC(ThreadLevel);

    level->n_elem = numthreads;
    level->list = MEM_ALLOC_N(pthread_t, numthreads);

    tree->tree = realloc(tree->tree, sizeof(tree->tree) + sizeof(ThreadLevel*));
    tree->tree[height] = level;

    ++height;
    numthreads = numthreads / 2;
  }

  tree->height = height;

  return tree;
}

void thread_tree_free(ThreadTree *tree)
{
  size_t i;

  if (tree) {
    for (i = 0; i < tree->height; i++)
      free(tree->tree[i]);

    free(tree->tree);
    free(tree);
  }
}

