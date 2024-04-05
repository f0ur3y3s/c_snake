/**
 * @file sll.c
 * @author Daniel Chung
 * @brief Implementation of threadsafe singly linked list
 *
 * TODO: additional notes
 *
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "../include/sll.h"

static sll_node_t * sll_node_create (void * p_data);

/**
 * @brief Creates a new singly linked list on heap
 *
 * @note cmp_func can be NULL
 *
 * @param destroy_node Function pointer to destroy node
 * @return sll_t*
 * @retval Pointer to new sll on success
 * @retval NULL on failure
 */
// sll_t * sll_create (int (*cmp_func)(void *, void *))
sll_t * sll_create (sll_cmp_f cmp_func)
{
    sll_t * p_new_sll = NULL;

    // if (NULL == cmp_func)
    // {
    //     (void)fprintf(stderr, "Compare function cannot be NULL\n");
    //     goto EXIT;
    // }

    p_new_sll = (sll_t *)calloc(1, sizeof(sll_t));

    if (NULL == p_new_sll)
    {
        perror("Failed to allocate memory for new sll\n");
        goto EXIT;
    }

    p_new_sll->p_head        = NULL;
    p_new_sll->p_tail        = NULL;
    p_new_sll->size          = 0;
    p_new_sll->node_cmp_func = cmp_func;

    if (0 != pthread_rwlock_init(&p_new_sll->sll_rwlock, NULL))
    {
        perror("Failed to initialize rwlock\n");
        free(p_new_sll);
        p_new_sll = NULL;
    }

EXIT:
    return (p_new_sll);
}

/**
 * @brief Destroys a singly linked list
 *
 * @note Sets sll pointer to NULL after destruction
 * @note You must free data in the singly linked list separately
 *
 * @param p_sll Pointer to singly linked list
 * @return int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_RWLOCK if failed to destroy rwlock
 *
 */
int sll_destroy (sll_t ** pp_sll)
{
    int status = SLL_GENERAL;

    if ((NULL == pp_sll) || (NULL == *pp_sll))
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&((*pp_sll)->sll_rwlock)))
    {
        perror("Failed to acquire write lock\n");
        status = SLL_RWLOCK;
        goto EXIT;
    }

    while (NULL != (*pp_sll)->p_head)
    {
        sll_node_t * p_current = (*pp_sll)->p_head;
        (*pp_sll)->p_head      = p_current->p_next;
        free(p_current);
        p_current = NULL;
    }

    if (0 != pthread_rwlock_unlock(&((*pp_sll)->sll_rwlock)))
    {
        perror("Failed to release write lock\n");
        status = SLL_RWLOCK;
        goto EXIT;
    }

    if (0 != pthread_rwlock_destroy(&((*pp_sll)->sll_rwlock)))
    {
        perror("Failed to destroy rwlock\n");
        status = SLL_RWLOCK;
        goto EXIT;
    }

    free(*pp_sll);
    *pp_sll = NULL;
    status  = SLL_OK;

EXIT:
    return status;
}

/**
 * @brief Prepends a new node to the singly linked list
 *
 * @param p_sll Pointer to singly linked list
 * @param p_data Pointer to data to prepend
 * @param b_is_locked Whether to acquire the write lock
 *
 * @returns int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll or p_data is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_ALLOC if failed to allocate memory for new node
 *
 */
int sll_prepend (sll_t * p_sll, void * p_data, const bool b_is_locked)
{
    int status = SLL_GENERAL;

    if ((NULL == p_sll) || (NULL == p_data))
    {
        (void)fprintf(stderr,
                      "Pointer to singly linked list or data cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to acquire write lock\n");
            status = SLL_RWLOCK;
            goto EXIT;
        }
    }

    sll_node_t * p_new_node = sll_node_create(p_data);

    if (NULL == p_new_node)
    {
        (void)fprintf(stderr, "Failed to create new node\n");
        status = SLL_ALLOC;

        goto EXIT_UNLOCK;
    }

    if (NULL == p_sll->p_head)
    {
        p_sll->p_tail = p_new_node;
    }

    p_new_node->p_next = p_sll->p_head;
    p_sll->p_head      = p_new_node;
    p_sll->size++;
    status = SLL_OK;

EXIT_UNLOCK:
    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to release write lock\n");
            status = SLL_RWLOCK;
            goto EXIT;
        }
    }

EXIT:
    return status;
}

/**
 * @brief Appends a new node to the singly linked list
 *
 * @param p_sll Pointer to singly linked list
 * @param p_data Pointer to data to append
 * @param b_is_locked Whether to acquire the write lock
 * @return int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll or p_data is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_ALLOC if failed to allocate memory for new node
 * @retval SLL_OK if successful
 *
 */
int sll_append (sll_t * p_sll, void * p_data, const bool b_is_locked)
{
    int status = SLL_GENERAL;

    if ((NULL == p_sll) || (NULL == p_data))
    {
        (void)fprintf(stderr,
                      "Pointer to singly linked list or data cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
        {
            (void)fprintf(stderr, "Failed to acquire write lock\n");
            status = SLL_RWLOCK;
            goto EXIT;
        }
    }

    sll_node_t * p_new_node = sll_node_create(p_data);

    if (NULL == p_new_node)
    {
        (void)fprintf(stderr, "Failed to create new node\n");
        status = SLL_ALLOC;

        goto EXIT_UNLOCK;
    }

    if (NULL == p_sll->p_head)
    {
        p_sll->p_head = p_new_node;
    }
    else
    {
        p_sll->p_tail->p_next = p_new_node;
    }

    p_sll->p_tail = p_new_node;
    p_sll->size++;

    status = SLL_OK;

EXIT_UNLOCK:
    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release write lock\n");
        status = SLL_RWLOCK;
    }

EXIT:
    return status;
}

/**
 * @brief Inserts a new node to the singly linked list at a specific index
 *
 * @param p_sll Pointer to singly linked list
 * @param p_data Pointer to data to insert
 * @param index Index to insert
 * @return int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll or p_data is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_ALLOC if failed to allocate memory for new node
 * @retval SLL_BOUNDS if index is out of bounds
 */
int sll_insert (sll_t * p_sll, void * p_data, const size_t index)
{
    int status = SLL_GENERAL;

    if ((NULL == p_sll) || (NULL == p_data))
    {
        (void)fprintf(stderr,
                      "Pointer to singly linked list or data cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire write lock\n");
        status = SLL_RWLOCK;
        goto EXIT;
    }

    if ((0 > index) || (SIZE_MAX <= index) || (index > p_sll->size))
    {
        (void)fprintf(stderr, "Index out of bounds\n");
        status = SLL_BOUNDS;

        goto EXIT_UNLOCK;
    }

    if (0 == index)
    {
        status = sll_prepend(p_sll, p_data, true);
    }
    else if (p_sll->size == index)
    {
        status = sll_append(p_sll, p_data, true);
    }
    else
    {
        sll_node_t * p_new_node = sll_node_create(p_data);

        if (NULL == p_new_node)
        {
            (void)fprintf(stderr, "Failed to create new node\n");
            status = SLL_ALLOC;

            goto EXIT_UNLOCK;
        }

        sll_node_t * p_current = p_sll->p_head;
        size_t       idx       = 0;

        // skip over to the nodes before the index
        while (idx < index - 1)
        {
            p_current = p_current->p_next;
            idx++;
        }

        p_new_node->p_next = p_current->p_next;
        p_current->p_next  = p_new_node;
        p_sll->size++;
    }

    status = SLL_OK;

EXIT_UNLOCK:
    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release write lock\n");
        status = SLL_RWLOCK;
    }

EXIT:
    return status;
}

/**
 * @brief Removes the first node
 *
 * @param p_sll Pointer to singly linked list
 * @param b_is_locked Whether to acquire the write lock
 * @return int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_BOUNDS if singly linked list is empty
 * @retval SLL_OK if successful
 *
 */
int sll_preremove (sll_t * p_sll, const bool b_is_locked)
{
    int status = SLL_GENERAL;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to acquire write lock\n");
            status = SLL_RWLOCK;
            goto EXIT;
        }
    }

    if (NULL == p_sll->p_head)
    {
        (void)fprintf(stderr, "Singly linked list is empty\n");
        status = SLL_BOUNDS;
        goto EXIT_UNLOCK;
    }

    sll_node_t * p_current = p_sll->p_head;
    p_sll->p_head          = p_current->p_next;
    free(p_current);
    p_current = NULL;
    p_sll->size--;

    status = SLL_OK;

EXIT_UNLOCK:
    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to release write lock\n");
            status = SLL_RWLOCK;
        }
    }

EXIT:
    return status;
}

/**
 * @brief Removes the last node
 *
 * @param p_sll Pointer to singly linked list
 * @param b_is_locked Whether to acquire the write lock
 * @return int
 * @retval SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_BOUNDS if singly linked list is empty
 *
 */
int sll_postremove (sll_t * p_sll, const bool b_is_locked)
{
    int status = SLL_GENERAL;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to acquire write lock\n");
            status = SLL_RWLOCK;
            goto EXIT;
        }
    }

    if (NULL == p_sll->p_head)
    {
        (void)fprintf(stderr, "Singly linked list is empty\n");
        status = SLL_BOUNDS;
        goto EXIT_UNLOCK;
    }

    sll_node_t * p_current = p_sll->p_head;
    sll_node_t * p_prev    = NULL;

    // skip over to the last node
    while (NULL != p_current->p_next)
    {
        p_prev    = p_current;
        p_current = p_current->p_next;
    }

    if (NULL == p_prev)
    {
        p_sll->p_head = NULL;
    }
    else
    {
        p_prev->p_next = NULL;
    }

    p_sll->p_tail = p_prev;
    free(p_current);
    p_current = NULL;
    p_sll->size--;

    status = SLL_OK;

EXIT_UNLOCK:
    if (!b_is_locked)
    {
        if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
        {
            perror("Failed to release write lock\n");
            status = SLL_RWLOCK;
        }
    }

EXIT:
    return status;
}

/**
 * @brief Removes a node at a specific index
 *
 * @param p_sll Pointer to singly linked list
 * @param index Index to remove
 * @return int SLL_OK on success (0)
 * @retval nonzero otherwise
 * @retval SLL_NULL if p_sll is NULL
 * @retval SLL_RWLOCK if failed to acquire/release write lock
 * @retval SLL_BOUNDS if index is out of bounds
 *
 */
int sll_remove (sll_t * p_sll, const size_t index)
{
    int status = SLL_GENERAL;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        status = SLL_NULL;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire write lock\n");
        status = SLL_RWLOCK;
        goto EXIT;
    }

    if ((0 > index) || (SIZE_MAX <= index) || (index >= p_sll->size))
    {
        (void)fprintf(stderr, "Index out of bounds\n");
        status = SLL_BOUNDS;

        goto EXIT_UNLOCK;
    }

    if (0 == index)
    {
        status = sll_preremove(p_sll, true);
    }
    else if (p_sll->size - 1 == index)
    {
        status = sll_postremove(p_sll, true);
    }
    else
    {
        sll_node_t * p_current = p_sll->p_head;
        sll_node_t * p_prev    = NULL;
        size_t       idx       = 0;

        while (idx < index)
        {
            p_prev    = p_current;
            p_current = p_current->p_next;
            idx++;
        }

        p_prev->p_next = p_current->p_next;
        free(p_current);
        p_current = NULL;
        p_sll->size--;
    }

    status = SLL_OK;

EXIT_UNLOCK:
    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release write lock\n");
        status = SLL_RWLOCK;
    }

EXIT:
    return status;
}

/**
 * @brief Gets the data at a specific index
 *
 * @param p_sll Pointer to singly linked list
 * @param index Index to get
 * @return void*
 * @retval Pointer to data on success
 * @retval NULL on failure
 */
void * sll_get (sll_t * p_sll, const size_t index)
{
    void * p_data = NULL;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire read lock\n");
        goto EXIT;
    }

    if ((0 > index) || (SIZE_MAX <= index) || (index >= p_sll->size))
    {
        (void)fprintf(stderr, "Index out of bounds\n");
        goto EXIT_UNLOCK;
    }

    sll_node_t * p_current = p_sll->p_head;
    size_t       idx       = 0;

    while (idx < index)
    {
        p_current = p_current->p_next;
        idx++;
    }

    p_data = p_current->p_data;

EXIT_UNLOCK:
    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release read lock\n");
        p_data = NULL;
    }

EXIT:
    return p_data;
}

/**
 * @brief Checks if data is in the singly linked list
 *
 * @param p_sll Pointer to singly linked list
 * @param p_data Pointer to data to check
 * @retval true if data is in the singly linked list
 * @retval false if data is not in the singly linked list
 */
bool sll_is_in (sll_t * p_sll, const void * p_data)
{
    bool b_is_in = false;

    if ((NULL == p_sll) || (NULL == p_data))
    {
        (void)fprintf(stderr,
                      "Pointer to singly linked list or data cannot be NULL\n");
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire read lock\n");
        goto EXIT;
    }

    sll_node_t * p_current = p_sll->p_head;

    while (NULL != p_current)
    {
        if (0 == p_sll->node_cmp_func(&(p_current->p_data), &p_data))
        {
            b_is_in = true;
            break;
        }

        p_current = p_current->p_next;
    }

    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release read lock\n");
        // b_is_in = false;
    }

EXIT:
    return b_is_in;
}

/**
 * @brief Checks if singly linked list is empty
 *
 * @param p_sll Pointer to singly linked list
 * @retval true if singly linked list is empty
 * @retval false if singly linked list is not empty
 */
bool sll_is_empty (sll_t * p_sll)
{
    bool b_is_empty = false;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire read lock");
        goto EXIT;
    }

    b_is_empty = (NULL == p_sll->p_head);

    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release read lock");
        // b_is_empty = false;
    }

EXIT:
    return b_is_empty;
}

/**
 * @brief Gets the size of the singly linked list
 *
 * @param p_sll Pointer to singly linked list
 * @return size_t
 * @retval Size of singly linked list
 */
size_t sll_size (sll_t * p_sll)
{
    size_t size = 0;

    if (NULL == p_sll)
    {
        (void)fprintf(stderr, "Pointer to singly linked list cannot be NULL\n");
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to acquire read lock");
        goto EXIT;
    }

    size = p_sll->size;

    if (0 != pthread_rwlock_unlock(&(p_sll->sll_rwlock)))
    {
        perror("Failed to release read lock");
        size = 0;
    }

EXIT:
    return size;
}

/**
 * @brief Creates a new singly linked list node on heap
 *
 * @param p_data Pointer to data to store in node
 * @retval sll_node_t* Pointer to new node on success
 * @retval NULL on failure
 */
static sll_node_t * sll_node_create (void * p_data)
{
    sll_node_t * p_new_node = NULL;

    if (NULL == p_data)
    {
        (void)fprintf(stderr, "Pointer to data cannot be NULL\n");
        goto EXIT;
    }

    p_new_node = (sll_node_t *)calloc(1, sizeof(sll_node_t));

    if (NULL == p_new_node)
    {
        (void)fprintf(stderr, "Failed to allocate memory for new node\n");
        goto EXIT;
    }

    p_new_node->p_data = p_data;
    p_new_node->p_next = NULL;

EXIT:
    return p_new_node;
}

/*** end of file ***/
