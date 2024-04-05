#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "term.h"
#include "game.h"

typedef enum movement_keys_t
{
    MOVEMENT_KEY_UP    = 119,
    MOVEMENT_KEY_DOWN  = 115,
    MOVEMENT_KEY_RIGHT = 100,
    MOVEMENT_KEY_LEFT  = 97
} movement_keys_t;

#define BOARD_SIZE 40

#endif // MAIN_H

/*** end of file ***/