#include "../include/entity.h"

entity_t * entity_create (point_t pos, point_t dir, entity_type_t type)
{
    entity_t * p_new_entity = NULL;

    p_new_entity = (entity_t *)calloc(1, sizeof(entity_t));

    if (NULL == p_new_entity)
    {
        perror("malloc");
        goto EXIT;
    }

    p_new_entity->pos          = pos;
    p_new_entity->dir          = dir;
    p_new_entity->entity_type  = type;
    p_new_entity->is_deletable = false;

    switch (type)
    {
        case EMPTY:
            break;
        case PLAYER:
            break;
        case FOOD:
            break;
        default:
            goto EXIT;
    }

EXIT:
    return (p_new_entity);
}

/*** end of file ***/
