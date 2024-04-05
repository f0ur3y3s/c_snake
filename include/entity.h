#ifndef ENTITY_H
#define ENTITY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "point.h"

typedef enum entity_type_t
{
    EMPTY = 0,
    PLAYER,
    FOOD,
} entity_type_t;

typedef struct entity_t
{
    point_t       pos;
    point_t       dir;
    entity_type_t entity_type;
    bool          is_deletable;
} entity_t;

entity_t * entity_create (point_t pos, point_t dir, entity_type_t type);

#endif // ENTITY_H