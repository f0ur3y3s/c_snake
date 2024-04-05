/**
 * @file dyn_arr.h
 * @author Daniel Chung
 * @brief Header file for dynamic array
 * @version 0.1
 * @date 2024-02-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <stdint.h>

/**
 * @brief Enumeration for dynamic array constants
 *
 */
typedef enum dyn_arr_constants_t
{
    ARRAY_RESIZE    = 2,
    DEFAULT_ARR_CAP = 8,
} dyn_arr_constants_t;

/**
 * @brief Enumeration for dynamic array error codes
 *
 */
typedef enum dyn_arr_error_t
{
    DYN_ARR_GENERAL = -1,
    DYN_ARR_OK      = 0,
    DYN_ARR_NULL,
    DYN_ARR_ALLOC,
    DYN_ARR_BOUNDS,
    DYN_ARR_INVALID,
    DYN_ARR_ERROR,
    DYN_ARR_RWLOCK,
} dyn_arr_error_t;

/**
 * @brief Dynamic array structure
 *
 * Holds a pointer to an array of void pointers, the size of elements, and the
 * capacity of the array
 * @param dyn_arr_t::pp_arr Pointer to array of void pointers
 * @param dyn_arr_t::size Count of elements in array
 * @param dyn_arr_t::capacity Capacity of array
 */
typedef struct dyn_arr_t
{
    void **          pp_arr;
    size_t           size;
    size_t           capacity;
    pthread_rwlock_t da_rwlock;
} dyn_arr_t;

/**
 * @brief Creates a dynamic array on heap
 *
 * @param capacity Initial capacity of the array
 * @retval dyn_arr_t* Pointer to dynamic array on success
 * @retval NULL on failure
 */
dyn_arr_t * dyn_arr_create (const size_t capacity);
/**
 * @brief Destroys a dynamic array
 *
 * @note Sets the pointer of dynamic array to NULL after destruction.
 *
 * @param pp_dyn_arr Pointer to dynamic array
 * @retval DYN_ARR_OK on success (0)
 * @retval non-zero on failure
 * @retval DYN_ARR_NULL if p_dyn_arr is NULL
 * @retval DYN_ARR_RWLOCK if failed to destroy rwlock
 *
 */
int dyn_arr_destroy (dyn_arr_t ** pp_dyn_arr);
/**
 * @brief Appends data to the end of the dynamic array
 *
 * @note Appends data to a dynamic array and resizes the array if necessary by
 * doubling the previous capacity.
 *
 * @param p_dyn_arr Pointer to dynamic array
 * @param p_data Pointer to data to be appended
 * @retval DYN_ARR_OK on success (0)
 * @retval non-zero on failure
 * @retval DYN_ARR_NULL if p_dyn_arr or p_data is NULL
 * @retval DYN_ARR_RWLOCK if failed to acquire write lock
 * @retval DYN_ARR_ERROR if failed to resize array
 * @retval Resize errors if resize failed
 *
 */
int dyn_arr_append (dyn_arr_t * p_dyn_arr, void * p_data);

/**
 * @brief Removes a value from the dynamic array
 *
 * @param p_dyn_arr Pointer to dynamic array
 * @param index Index of value to remove
 * @retval DYN_ARR_OK on success (0)
 * @retval non-zero on failure
 * @retval DYN_ARR_NULL if p_dyn_arr is NULL
 * @retval DYN_ARR_BOUNDS if index is out of bounds
 * @retval DYN_ARR_INVALID if array is empty
 * @retval DYN_ARR_RWLOCK if failed to acquire write lock
 * @retval DYN_ARR_RWLOCK if failed to release write lock
 *
 */
int dyn_arr_remove (dyn_arr_t * p_dyn_arr, const size_t index);
/**
 * @brief Gets a value from the dynamic array
 *
 * @param p_dyn_arr Pointer to dynamic array
 * @param index Index of value to get
 * @retval void* Pointer to data on success
 * @retval NULL on failure
 */
void * dyn_arr_get (dyn_arr_t * p_dyn_arr, const size_t index);
/**
 * @brief Sets a value in the dynamic array
 *
 * @param p_dyn_arr Pointer to dynamic array
 * @param p_data Pointer to data to set
 * @param index Index in dynamic arary to set
 * @retval DYN_ARR_OK on success (0)
 * @retval non-zero on failure
 * @retval DYN_ARR_NULL if p_dyn_arr or p_data is NULL
 * @retval DYN_ARR_BOUNDS if index is out of bounds
 * @retval DYN_ARR_RWLOCK if failed to acquire write lock
 * @retval Resize errors if resize failed
 */
int dyn_arr_set (dyn_arr_t * p_dyn_arr, void * p_data, const size_t index);

#endif // DYN_ARR_H

/*** end of file ***/
