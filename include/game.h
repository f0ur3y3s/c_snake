#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include "dyn_arr.h"
#include "sll.h"
#include "entity.h"
#include "point.h"
#include "term.h"

#define MAT_SIZE         11
#define GAME_ICON_EMPTY  ". "
#define GAME_ICON_PLAYER "[]"
#define GAME_ICON_FOOD   "()"
#define UPPER_LEFT       "┌"
#define UPPER_RIGHT      "┐"
#define LOWER_LEFT       "└"
#define LOWER_RIGHT      "┘"
#define HORIZONTAL       "──"
#define VERTICAL         "│"
#define OFFSET           2

typedef struct game_tile_t
{
    entity_type_t tile_type;
} game_tile_t;

typedef struct game_t
{
    dyn_arr_t *   p_entity_arr;
    sll_t *       p_snake;
    game_tile_t * p_tile_matrix;
    size_t        game_size;
    int           score;
} game_t;

game_t * game_init (size_t game_size);
void     game_destroy (game_t ** pp_game);
void     game_print_tiles (game_t * p_game);
void     game_turn_player (game_t * p_game, point_t dir);
bool     game_tick (game_t * p_game);
bool     game_should_run (void);

#endif // GAME_H

/*** end of file ***/
