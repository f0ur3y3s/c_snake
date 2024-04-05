/**
 * @file dyn_arr.c
 * @author Daniel Chung
 * @brief Threadsafe dynamic array implementation. This is an abstract data type
 * array.
 * @version 0.1
 * @date 2024-02-12
 *
 * This is a threadsafe implementation of a dynamic array. It uses a read-write
 * lock to allow multiple readers or a single writer to access the array. The
 * array resizes by doubling the capacity when the array is full.
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "../include/dyn_arr.h"

/**
 * @brief Resizes a dynamic array
 *
 * @note THIS FUNCTION IS NOT THREADSAFE!!!
 * @note You must acquire a write lock before calling this function
 *
 * @param p_dyn_arr Pointer to dynamic array
 * @param new_capacity New capacity of the array
 * @retval DYN_ARR_OK on success (0)
 * @retval non-zero on failure
 * @retval DYN_ARR_NULL if p_dyn_arr is NULL
 * @retval DYN_ARR_BOUNDS if new_capacity is less than or equal to 0
 * @retval DYN_ARR_ALLOC if failed to allocate memory for new array
 *
 */
static int dyn_arr_resize (dyn_arr_t * p_dyn_arr, const size_t new_capacity);


dyn_arr_t * dyn_arr_create (const size_t capacity)
{
    dyn_arr_t * p_new_arr = (dyn_arr_t *)calloc(1, sizeof(dyn_arr_t));

    if (NULL == p_new_arr)
    {
        perror("malloc");
        goto EXIT;
    }

    p_new_arr->capacity = capacity;
    p_new_arr->size    = 0;
    p_new_arr->pp_arr   = (void **)calloc(p_new_arr->capacity, sizeof(void *));

    if (NULL == p_new_arr->pp_arr)
    {
        perror("malloc");
        free(p_new_arr);
        p_new_arr = NULL;
        goto EXIT;
    }

    // initialize rwlock
    if (0 != pthread_rwlock_init(&(p_new_arr->da_rwlock), NULL))
    {
        perror("Failed to init rwlock for hashtable\n");
        free(p_new_arr);
        p_new_arr = NULL;
        goto EXIT;
    }

EXIT:
    return p_new_arr;
}


int dyn_arr_destroy (dyn_arr_t ** pp_dyn_arr)
{
    int status = DYN_ARR_GENERAL;

    if ((NULL == pp_dyn_arr) || (NULL == *pp_dyn_arr))
    {
        (void)fprintf(stderr, "Dynamic array is NULL\n");
        status = DYN_ARR_NULL;
        goto EXIT;
    }

    if (0 != pthread_rwlock_destroy(&((*pp_dyn_arr)->da_rwlock)))
    {
        perror("Failed to destroy rwlock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
        goto EXIT;
    }

    if (NULL != (*pp_dyn_arr)->pp_arr)
    {
        free((*pp_dyn_arr)->pp_arr);
    }

    free((*pp_dyn_arr));
    *pp_dyn_arr = NULL;
    status      = DYN_ARR_OK;

EXIT:
    return status;
}


int dyn_arr_append (dyn_arr_t * p_dyn_arr, void * p_data)
{
    int status = DYN_ARR_GENERAL;

    if ((NULL == p_dyn_arr) || (NULL == p_data))
    {
        (void)fprintf(stderr, "Invalid arguments\n");
        status = DYN_ARR_NULL;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to acquire write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
        goto EXIT;
    }

    if (p_dyn_arr->size == p_dyn_arr->capacity)
    {
        status = dyn_arr_resize(p_dyn_arr, p_dyn_arr->capacity * ARRAY_RESIZE);

        if (DYN_ARR_OK != status)
        {
            (void)fprintf(stderr, "Failed to resize array!!\n");

            if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
            {
                perror("Failed to release write lock for dyn_arr\n");
                status = DYN_ARR_RWLOCK;
            }

            goto EXIT;
        }
    }

    p_dyn_arr->pp_arr[p_dyn_arr->size] = p_data;
    p_dyn_arr->size++;
    status = DYN_ARR_OK;

    if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to release write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
    }

EXIT:
    return status;
}


void * dyn_arr_get (dyn_arr_t * p_dyn_arr, const size_t index)
{
    void * p_data = NULL;

    if (NULL == p_dyn_arr)
    {
        (void)fprintf(stderr, "Dynamic array is NULL\n");
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to acquire read lock for dyn_arr\n");
        goto EXIT;
    }

    if ((0 > index) || (p_dyn_arr->capacity <= index))
    {
        (void)fprintf(stderr, "Invalid index\n");

        if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
        {
            perror("Failed to release read lock for dyn_arr\n");
        }

        goto EXIT;
    }

    p_data = p_dyn_arr->pp_arr[index];

    if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to release read lock for dyn_arr\n");
        p_data = NULL;
    }

EXIT:
    return p_data;
}


int dyn_arr_set (dyn_arr_t * p_dyn_arr, void * p_data, const size_t index)
{
    int status = DYN_ARR_GENERAL;

    if (NULL == p_dyn_arr)
    {
        (void)fprintf(stderr, "Dynamic array is NULL\n");
        status = DYN_ARR_NULL;
        goto EXIT;
    }

    if ((0 > index) || (SIZE_MAX <= index))
    {
        (void)fprintf(stderr, "Invalid index\n");
        status = DYN_ARR_BOUNDS;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to acquire write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
        goto EXIT;
    }

    while (p_dyn_arr->capacity <= index)
    {
        status = dyn_arr_resize(p_dyn_arr, p_dyn_arr->capacity * ARRAY_RESIZE);

        if (DYN_ARR_OK != status)
        {
            (void)fprintf(stderr, "Failed to resize array!!\n");
            status = DYN_ARR_ERROR;

            if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
            {
                perror("Failed to release write lock for dyn_arr\n");
                status = DYN_ARR_RWLOCK;
            }

            goto EXIT;
        }
    }

    if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to release write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
        goto EXIT;
    }

    p_dyn_arr->pp_arr[index] = p_data;
    p_dyn_arr->size++;
    status = DYN_ARR_OK;

EXIT:
    return status;
}


int dyn_arr_remove (dyn_arr_t * p_dyn_arr, const size_t index)
{
    int status = DYN_ARR_GENERAL;

    if (0 != pthread_rwlock_wrlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to acquire write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
        goto EXIT;
    }

    if ((0 > index) || (p_dyn_arr->capacity <= index))
    {
        status = DYN_ARR_BOUNDS;
        goto EXIT;
    }

    if (0 == p_dyn_arr->size)
    {
        status = DYN_ARR_INVALID;

        if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
        {
            perror("Failed to release write lock for dyn_arr\n");
            status = DYN_ARR_RWLOCK;
        }

        goto EXIT;
    }

    p_dyn_arr->pp_arr[index] = NULL;

    // shift over values
    for (size_t idx = index; idx < p_dyn_arr->size - 1; idx++)
    {
        p_dyn_arr->pp_arr[idx] = p_dyn_arr->pp_arr[idx + 1];
    }

    p_dyn_arr->size--;
    status = DYN_ARR_OK;

    if (0 != pthread_rwlock_unlock(&(p_dyn_arr->da_rwlock)))
    {
        perror("Failed to release write lock for dyn_arr\n");
        status = DYN_ARR_RWLOCK;
    }

EXIT:
    return status;
}


static int dyn_arr_resize (dyn_arr_t * p_dyn_arr, const size_t new_capacity)
{
    int status = DYN_ARR_GENERAL;

    if (NULL == p_dyn_arr)
    {
        (void)fprintf(stderr, "Dynamic array is NULL\n");
        status = DYN_ARR_NULL;
        goto EXIT;
    }

    if (0 >= new_capacity)
    {
        (void)fprintf(stderr, "Invalid new capacity\n");
        status = DYN_ARR_BOUNDS;
        goto EXIT;
    }

    void ** pp_new_arr = (void **)calloc(new_capacity, sizeof(void *));

    if (NULL == pp_new_arr)
    {
        perror("calloc");
        status = DYN_ARR_ALLOC;
        goto EXIT;
    }

    // set all to NULL
    for (int cap_idx = 0; cap_idx < new_capacity; cap_idx++)
    {
        pp_new_arr[cap_idx] = NULL;
    }

    // copy over values from old array to new array
    for (int cap_idx = 0; cap_idx < p_dyn_arr->capacity; cap_idx++)
    {
        pp_new_arr[cap_idx] = p_dyn_arr->pp_arr[cap_idx];
    }

    free(p_dyn_arr->pp_arr);
    p_dyn_arr->pp_arr   = pp_new_arr;
    p_dyn_arr->capacity = new_capacity;
    status              = DYN_ARR_OK;

EXIT:
    return status;
}

/*** end of file ***/
