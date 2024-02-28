#pragma once
#include <stddef.h>

typedef struct observable_s observable_t;
typedef struct observer_s observer_t;
typedef void (*observer_callback_t)(void *data);

observable_t *observable_init(void);
void observable_term(observable_t *observable);

observer_t *observable_on(observable_t *observable, observer_callback_t);
void observable_off(observable_t *observable, observer_t *observer);

void observable_notify(observable_t *observable, void *data);
