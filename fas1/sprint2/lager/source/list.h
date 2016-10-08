#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

/**
 * @file
 * @author Andreas Olofsson
 * @version 1.0
 */

/**
 * @brief Represents an ordered list.
 */
typedef struct list list_t;

/**
 * @brief Represents an iterator for a list_t.
 */
typedef struct list_it list_it_t;

/**
 * @brief Creates a new list_t
 */
list_t* list_new();

/**
 * @brief Destroys a list_t, freeing any memory allocated for the list.
 *
 * Destroys the provided list and free's any memory allocated for it.
 * Doesn't free any memory alloc'd for the elements stored within the list,
 * they must be manually free'd be the user.
 * Any use of the provided list_t is invalid after calling list_destroy.
 *
 * @param list The list to destroy.
 */
void list_destroy(list_t* list);

/**
 * @brief Gives the number of elements in a list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param list
 * @return The number of elements in list.
 */
int list_length(list_t* list);

/**
 * @brief Appends elem to the end of the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param list The list to append to.
 * @param elem The element to append.
 */
void list_append(list_t* list, void* elem);

/**
 * @brief Prepends elem to the start of the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param list The list to prepend to.
 * @param elem The element to prepend.
 */
void list_prepend(list_t* list, void* elem);

/**
 * @brief Inserts elem into the list at the given index.
 *
 * Completes in @f$ O(n) @f$ time.
 *
 * @param list The list to insert into.
 * @param index The index to insert at.
 * @param elem The element to be inserted.
 *             Must be between 0 and the length of the list - 1.
 * @return Whether of not the insert was successful.
 */
bool list_insert(list_t* list, int index, void* elem);

/**
 * @brief Removes an element at the given index.
 *
 * Returns the removed element in the provided elem pointer if the pointer is not NULL.
 *
 * Completes in @f$ O(n) @f$ time where @f$ n @f$ is index.
 *
 * @param list The list to remove from.
 * @param index The index of the element to remove.
 * @param elem 
 */
bool list_remove(list_t* list, int index, void** elem);

/**
 * @brief Gives the element at the given index.
 *
 * Completes in @f$ O(n) @f$ time where @f$ n @f$ is index.
 *
 * @param list The list to get from.
 * @param index The index to get from.
 * @return A pointer to the element.
 *         The value at the pointer may be modified in order to replace an element in-place.
 */
void** list_get(list_t* list, int index);

/**
 * @brief Gives the first element in the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param list The list to get from.
 */
void* list_first(list_t* list);

/**
 * @brief Gives the last element in the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param list The list to get from.
 */
void* list_last(list_t* list);

/**
 * @brief Creates a new iterator for the given list.
 *
 * The iterator starts at index 0 unless the list is empty.
 * The iterator is not valid if the list is modified after the iterator is created.
 *
 * @param list The list to iterate over.
 * @return The list iterator.
 */
list_it_t* list_it_new(list_t* list);

/**
 * @brief Destroys a list iterator and free's memory allocated for it.
 *
 * @param it The iterator to destroy.
 */
void list_it_destroy(list_it_t* it);

/**
 * @brief Determines whether or not the iterator is at the first element in the list.
 *
 * @param it The iterator to test.
 * @return If the iterator is at the first element in the list.
 */
bool list_it_first(list_it_t* it);

/**
 * @brief Determines whether or not the iterator is at the last element in the list.
 *
 * @param it The iterator to test.
 * @return If the iterator is at the list element in the list.
 */
bool list_it_last(list_it_t* it);

/**
 * @brief Gives the element the iterator is currently at.
 *
 * @param it The iterator to get from.
 * @return A pointer to the element the iterator is currently at.
 *         The value at the pointer may be modified without invalidating the iterator.
 */
void** list_it_current(list_it_t* it);

/**
 * @brief Moves the iterator to the next element in the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param it The iterator to update.
 * @return A pointer to the element the iterator was at before the function call;
 */
void** list_it_next(list_it_t* it);


/**
 * @brief Moves the iterator to the previous element in the list.
 *
 * Completes in @f$ O(1) @f$ time.
 *
 * @param it The iterator to update.
 * @return A pointer to the element the iterator was at before the function call;
 */
void** list_it_prev(list_it_t* it);

/**
 * @brief Moves the iterator to the gives index in the list.
 *
 * Completes in @f$ O(n) @f$ time.
 *
 * @param it The iterator to update.
 * @param index The index to set the iterator at.
 */
void list_it_seek(list_it_t* it, int index);

/**
 * @brief Gives the current index of the given iterator.
 *
 * @param it The iterator to get the index from.
 */
int list_it_index(list_it_t* it);

#endif /* __LIST_H__ */


