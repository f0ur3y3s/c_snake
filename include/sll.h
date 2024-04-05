/**
 * @file sll.h
 * @author Daniel Chung
 * @brief Threadsafe singly linked list
 * @version 0.1
 * @date 2024-03-06
 *
 * Allows duplicates to be put in, not a set
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SLL_H
#define SLL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>

typedef struct sll_node_t
{
    void *              p_data;
    struct sll_node_t * p_next;
} sll_node_t;

typedef int (*sll_cmp_f)(void *, void *);

typedef struct sll_t
{
    sll_node_t *     p_head;
    sll_node_t *     p_tail;
    size_t           size;
    pthread_rwlock_t sll_rwlock;
    sll_cmp_f        node_cmp_func;
    // int (*node_cmp_func)(void *, void *);
} sll_t;

typedef enum sll_error_t
{
    SLL_GENERAL = -1,
    SLL_OK      = 0,
    SLL_NULL,
    SLL_ALLOC,
    SLL_INVALID,
    SLL_BOUNDS,
    SLL_RWLOCK,
} sll_error_t;

// sll_t * sll_create (int (*cmp_func)(void *, void *));
sll_t * sll_create (sll_cmp_f cmp_func);
int     sll_destroy (sll_t ** pp_sll);
int     sll_prepend (sll_t * p_sll, void * p_data, const bool b_is_locked);
int     sll_append (sll_t * p_sll, void * p_data, const bool b_is_locked);
int     sll_insert (sll_t * p_sll, void * p_data, const size_t index);
int     sll_preremove (sll_t * p_sll, const bool b_is_locked);
int     sll_postremove (sll_t * p_sll, const bool b_is_locked);
int     sll_remove (sll_t * p_sll, const size_t index);
void *  sll_get (sll_t * p_sll, const size_t index);
bool    sll_is_in (sll_t * p_sll, const void * p_data);
bool    sll_is_empty (sll_t * p_sll);
size_t  sll_size (sll_t * p_sll);

#endif // SLL_H

/*** end of file ***/
