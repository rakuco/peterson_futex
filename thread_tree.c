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
    free(level->turn);
    free(level);
  }
}

ThreadLevel *thread_level_new(size_t numthreads)
{
  ThreadLevel *level;

  level = MEM_ALLOC(ThreadLevel);

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

size_t thread_tree_get_turn_pos(size_t thread_id)
{
  return (thread_id % 2 ? (thread_id-1)/2 : thread_id/2);
}

ThreadTree *thread_tree_new(size_t numthreads)
{
  size_t height = 0;
  size_t onumthreads = numthreads;
  ThreadLevel *level;
  ThreadTree *tree;

  assert(numthreads > 0);

  tree = MEM_ALLOC(ThreadTree);
  tree->tree = NULL;
  tree->thread_list = MEM_ALLOC_N(pthread_t, numthreads);

  numthreads = (numthreads % 2 ? numthreads - 1 : numthreads);

  while (numthreads) {
    level = thread_level_new(onumthreads);

    tree->tree = realloc(tree->tree, height*sizeof(tree->tree) + sizeof(ThreadLevel*));
    tree->tree[height] = level;

    ++height;
    numthreads = numthreads / 2;
  }

  tree->height = height;

  return tree;
}

void thread_tree_show_interest(ThreadTree *tree, size_t level, size_t thread_id)
{
  size_t turn;

  assert(tree);
  assert(level < thread_tree_get_height(tree));

  turn = thread_tree_get_turn_pos(thread_id);

  tree->tree[level]->interested[thread_id] = 1;
  tree->tree[level]->turn[turn] = thread_id;
}
