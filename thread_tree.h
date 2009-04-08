/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#ifndef __THREAD_TREE_H
#define __THREAD_TREE_H

typedef struct
{
  size_t *interested;
  size_t n_elem;
  size_t turn;
} ThreadLevel;

typedef struct
{
  size_t height;
  pthread_t *thread_list;
  ThreadLevel **tree;
} ThreadTree;

void thread_level_free(ThreadLevel *level);
ThreadLevel *thread_level_new(size_t numthreads);

void thread_tree_free(ThreadTree *tree);
size_t thread_tree_get_height(ThreadTree *tree);
ThreadTree *thread_tree_new(size_t numthreads);

#endif /* __THREAD_TREE_H */
