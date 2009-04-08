/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#ifndef __THREAD_TREE_H
#define __THREAD_TREE_H

/**
 * Structure to represent a level in the thread tree.
 */
typedef struct
{
  size_t *interested; /**< Whether an element is interested in the critical region */
  size_t n_elem;      /**< Number of elements in this level */
  size_t *turn;       /**< Whether it's the turn of a given thread or its sibling's */
} ThreadLevel;

/**
 * Struct to represent the thread tree.
 */
typedef struct
{
  size_t height;            /**< Tree height */
  pthread_t *thread_list;   /**< List of all threads being run */
  ThreadLevel **tree;       /**< Array of arrays of levels in the tree */
} ThreadTree;

/**
 * Frees memory allocated to a given @p ThreadLevel.
 *
 * @param level The structure whose memory will be freed.
 */
void thread_level_free(ThreadLevel *level);

/**
 * Returns the position of a given @p thread_id in a ThreadLevel::turn.
 *
 * @param thread_id The number whose position will be returned.
 *
 * @return The position in the turn array.
 */
size_t thread_level_get_turn_pos(size_t thread_id);

/**
 * Creates a new @p ThreadLevel.
 *
 * @param numthreads The number of elements this level will have.
 *
 * @return A new @p ThreadLevel.
 */
ThreadLevel *thread_level_new(size_t numthreads);

/**
 * Frees memory allocated to a given @p ThreadTree.
 *
 * @param tree The structure whose memory will be freed.
 */
void thread_tree_free(ThreadTree *tree);

/**
 * Returns the height of a given tree.
 *
 * @param tree The @p ThreadTree being used.
 *
 * @return The height of the tree.
 */
size_t thread_tree_get_height(ThreadTree *tree);

/**
 * Creates a new @p ThreadTree.
 *
 * @param numthreads The number of threads the initial level of the tree has.
 *
 * @return A new @p ThreadTree.
 */
ThreadTree *thread_tree_new(size_t numthreads);

/**
 * Marks that a thread is interested in entering the critical region.
 *
 * @param tree A @p ThreadTree
 * @param level The level in the tree.
 * @param thread_id The thread number in the level.
 */ 
void thread_tree_show_interest(ThreadTree *tree, size_t level, size_t thread_id);

#endif /* __THREAD_TREE_H */
