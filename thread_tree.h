/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#ifndef __THREAD_TREE_H
#define __THREAD_TREE_H

typedef struct
{
  size_t n_elem;
  pthread_t *list;
} ThreadLevel;

typedef struct
{
  size_t height;
  ThreadLevel **tree;
} ThreadTree;

ThreadTree *thread_tree_new(size_t numthreads);

void thread_tree_free(ThreadTree *tree);

#endif /* __THREAD_TREE_H */
