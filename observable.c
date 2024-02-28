#include <assert.h>
#include <stdlib.h>

#include "observable.h"

typedef struct observer_s {
  observer_callback_t callback;
  struct observer_s *prev;
  struct observer_s *next;
} observer_t;

typedef struct observable_s {
  size_t length;
  observer_t *head;
  observer_t *tail;
} observable_t;

observer_t *observable_on(observable_t *list, observer_callback_t callback) {
  assert(list);

  observer_t *node = malloc(sizeof(observer_t));

  if (list->length) {
    node->prev = list->tail;
    list->tail->next = node;
  } else {
    node->prev = NULL;
    list->head = node;
  }

  node->callback = callback;

  node->next = NULL;
  list->tail = node;
  list->length++;

  return node;
}

/**
 * Remove observer from observable (unsubscribe).
 *
 * (Remove node operation.)
 *
 * Warning: There is nothing to enforce that `observer` belongs to `observable`.
 *
 *          Removing an `observer` originating from a different `observable`
 *          is undefined behavior.
 */
void observable_off(observable_t *list, observer_t *node) {
  assert(list);
  assert(node);

  // If list is not empty
  if (list->length) {

    // If head == tail
    if (list->length == 1) {
      // It is the only node, so null both pointers.
      // (since it will be released)
      list->head = NULL;
      list->tail = NULL;
    }
    // If node is the head (but not the tail):
    else if (node == list->head) {
      // Move the head forward.
      list->head = node->next;
      // Null the new head's `prev` pointer.
      // (because it's the head, nothing is before it)
      list->head->prev = NULL;
    }
    // If node is the tail (but not the head):
    else if (node == list->tail) {
      // Move the tail backward.
      list->tail = node->prev;
      // Null the new prev's `next` pointer.
      // (because it's the tail, nothing is after it)
      list->tail->next = NULL;
    }

    // Decrement the length.
    list->length--;
  }

  // Clean up the node pointer.
  node->prev = NULL;
  node->next = NULL;
  node->callback = NULL;
  free(node);
}

observable_t *observable_init(void) {
  observable_t *list = malloc(sizeof(observable_t));
  list->length = 0;
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void observable_term(observable_t *list) {
  if (!list) {
    return;
  }

  while (list->length) {
    observable_off(list, list->tail);
  }

  list->head = NULL;
  list->tail = NULL;
  free(list);
}

void observable_notify(observable_t *list, void *data) {
  for (observer_t *node = list->head; node && node->callback;
       node = node->next) {
    node->callback(data);
  }
}
