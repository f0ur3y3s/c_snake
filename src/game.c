#include "../include/game.h"

static struct timeval g_time_last = { 0 };
entity_t *            gp_snake    = NULL;
point_t               g_zero      = { 0 };

static void game_place_tile (game_t * p_game, point_t pos, entity_type_t type)
{
    if ((NULL == p_game) || (0 > pos.x) || (p_game->game_size <= pos.x)
        || (0 > pos.y) || (p_game->game_size <= pos.y))
    {
        goto EXIT;
    }

    p_game->p_tile_matrix[(pos.y * p_game->game_size) + pos.x].tile_type = type;

EXIT:
    return;
}

static int game_add_entity (game_t *      p_game,
                            point_t       pos,
                            point_t       dir,
                            entity_type_t type)
{
    int status = -1;

    if ((NULL == p_game) || (0 > pos.x) || (p_game->game_size <= pos.x)
        || (0 > pos.y) || (p_game->game_size <= pos.y))
    {
        goto EXIT;
    }

    entity_t * p_new_entity = entity_create(pos, dir, type);

    if (NULL == p_new_entity)
    {
        goto EXIT;
    }

    p_new_entity->score = 0;

    switch (type)
    {
        case PLAYER:;
            entity_t * p_next   = (entity_t *)sll_get(p_game->p_snake, 0);
            p_new_entity->dir.x = p_next->dir.x;
            p_new_entity->dir.y = p_next->dir.y;
            status = sll_prepend(p_game->p_snake, p_new_entity, false);
            break;
        case FOOD:
            p_new_entity->score = 1;
            p_new_entity->pos.x = pos.x;
            p_new_entity->pos.y = pos.y;
            status = dyn_arr_append(p_game->p_entity_arr, p_new_entity);
            break;
        default:
            break;
    }

    if (0 != status)
    {
        perror("dyn_arr_append");
        free(p_new_entity);
        p_new_entity = NULL;
        status       = -1;
        goto EXIT;
    }

EXIT:
    return (status);
}

static int snake_cmp_f (void * p_pos1, void * p_pos2)
{
    int status = -1;

    if ((NULL == p_pos1) || (NULL == p_pos2))
    {
        goto EXIT;
    }

    point_t * p_point1 = (point_t *)p_pos1;
    point_t * p_point2 = (point_t *)p_pos2;

    if ((p_point1->x == p_point2->x) && (p_point1->y == p_point2->y))
    {
        printf("Snake collision\n");
        exit(1);
        status = 0;
    }

EXIT:
    return (status);
}

game_t * game_init (size_t game_size)
{
    game_t * p_new_game = NULL;

    if (0 == game_size)
    {
        goto EXIT;
    }

    p_new_game = (game_t *)calloc(1, sizeof(game_t));

    if (NULL == p_new_game)
    {
        perror("malloc");
        goto EXIT;
    }

    p_new_game->score        = 0;
    p_new_game->game_size    = game_size;
    p_new_game->p_entity_arr = dyn_arr_create(DEFAULT_ARR_CAP);

    if (NULL == p_new_game->p_entity_arr)
    {
        perror("dyn_arr_create");
        free(p_new_game);
        p_new_game = NULL;
        goto EXIT;
    }

    p_new_game->p_snake = sll_create(snake_cmp_f);

    if (NULL == p_new_game->p_snake)
    {
        perror("dyn_arr_create");
        dyn_arr_destroy(&(p_new_game->p_entity_arr));
        free(p_new_game);
        p_new_game = NULL;
        goto EXIT;
    }

    p_new_game->p_tile_matrix = (game_tile_t *)calloc(
        p_new_game->game_size * p_new_game->game_size, sizeof(entity_t));

    if (NULL == p_new_game->p_tile_matrix)
    {
        perror("malloc");
        sll_destroy(&(p_new_game->p_snake));
        dyn_arr_destroy(&(p_new_game->p_entity_arr));
        free(p_new_game);
        p_new_game = NULL;
        goto EXIT;
    }

    for (uint8_t y_idx = 0; y_idx < game_size; y_idx++)
    {
        for (uint8_t x_idx = 0; x_idx < game_size; x_idx++)
        {
            point_t pos = { .x = x_idx, .y = y_idx };
            game_place_tile(p_new_game, pos, EMPTY);
        }
    }

    point_t pos = { .x = 2, .y = game_size / 2 };
    point_t dir = { .x = 1, .y = 0 };

    entity_t * p_snake_head = entity_create(pos, dir, PLAYER);
    sll_prepend(p_new_game->p_snake, p_snake_head, false);
    game_place_tile(p_new_game, pos, PLAYER);

    pos.x -= 1;
    game_add_entity(p_new_game, pos, g_zero, PLAYER);
    game_place_tile(p_new_game, pos, PLAYER);

    pos.x -= 1;
    game_add_entity(p_new_game, pos, g_zero, PLAYER);
    game_place_tile(p_new_game, pos, PLAYER);

    (void)gettimeofday(&g_time_last, NULL);
    srand(time(NULL));

    for (int start_food = 0; start_food < 5; start_food++)
    {
        pos.x = rand() % p_new_game->game_size;
        pos.y = rand() % p_new_game->game_size;
        game_add_entity(p_new_game, pos, g_zero, FOOD);
        game_place_tile(p_new_game, pos, FOOD);
    }
    // pos.x = rand() % p_new_game->game_size;
    // pos.y = rand() % p_new_game->game_size;
    // game_add_entity(p_new_game, pos, g_zero, FOOD);
    // game_place_tile(p_new_game, pos, FOOD);
EXIT:
    return (p_new_game);
}

void game_print_tiles (game_t * p_game)
{
    system("clear");

    for (uint8_t i = 0; i < p_game->game_size; i++)
    {
        if (0 == i)
        {
            (void)fprintf(stdout, UPPER_LEFT);
        }
        (void)fprintf(stdout, HORIZONTAL);
    }
    (void)fprintf(stdout, "%s\n", UPPER_RIGHT);

    for (uint8_t i = 0; i < p_game->game_size; i++)
    {
        (void)fprintf(stdout, VERTICAL);

        for (uint8_t j = 0; j < p_game->game_size; j++)
        {
            entity_type_t tile_type
                = p_game->p_tile_matrix[(i * p_game->game_size) + j].tile_type;

            switch (tile_type)
            {
                case EMPTY:
                    (void)fprintf(stdout, GAME_ICON_EMPTY);
                    break;
                case PLAYER:
                    (void)fprintf(stdout, GAME_ICON_PLAYER);
                    break;
                case FOOD:
                    (void)fprintf(stdout, GAME_ICON_FOOD);
                    break;
                default:
                    (void)fprintf(stdout, GAME_ICON_EMPTY);
                    break;
            }
        }

        (void)fprintf(stdout, "%s\n", VERTICAL);
    }

    for (uint8_t i = 0; i < p_game->game_size; i++)
    {
        if (0 == i)
        {
            (void)fprintf(stdout, LOWER_LEFT);
        }
        (void)fprintf(stdout, HORIZONTAL);
    }
    (void)fprintf(stdout, "%s\n", LOWER_RIGHT);

    (void)fprintf(stdout, "Score: %d\n", p_game->score);
}

void game_destroy (game_t ** pp_game)
{
    if ((NULL == pp_game) || (NULL == *pp_game))
    {
        goto EXIT;
    }

    // if (NULL != (*pp_game)->p_entity_arr)
    // {
    //     entity_t * p_temp_entity = NULL;

    //     size_t idx = 0;
    //     for (; idx < (*pp_game)->p_entity_arr->size; idx++)
    //     {
    //         p_temp_entity
    //             = (entity_t *)dyn_arr_get((*pp_game)->p_entity_arr, idx);
    //         free(p_temp_entity);
    //         p_temp_entity = NULL;
    //         dyn_arr_remove((*pp_game)->p_entity_arr, idx);
    //         idx--;
    //     }
    // }

    dyn_arr_destroy(&((*pp_game)->p_entity_arr));
    sll_destroy(&((*pp_game)->p_snake));

    if (NULL != (*pp_game)->p_tile_matrix)
    {
        free((*pp_game)->p_tile_matrix);
    }

    free(*pp_game);
    *pp_game = NULL;

EXIT:
    return;
}

void game_turn_player (game_t * p_game, point_t dir)
{
    point_t curr_dir = ((entity_t *)p_game->p_snake->p_tail->p_data)->dir;

    if (curr_dir.x + dir.x == 0 || curr_dir.y + dir.y == 0)
    {
        goto EXIT;
    }

    ((entity_t *)p_game->p_snake->p_tail->p_data)->dir.x = dir.x;
    ((entity_t *)p_game->p_snake->p_tail->p_data)->dir.y = dir.y;

EXIT:
    return;
}

bool game_tick (game_t * p_game)
{
    bool           should_update = false;
    struct timeval time_now;
    (void)gettimeofday(&time_now, NULL);

    uint64_t delta_time = (time_now.tv_sec - g_time_last.tv_sec) * 1000000
                          + time_now.tv_usec - g_time_last.tv_usec;

    if (100000 > delta_time)
    {
        goto EXIT;
    }

    g_time_last = time_now;

    // check if move will cause collision with itself
    entity_t * p_snake_head = (entity_t *)p_game->p_snake->p_tail->p_data;

    point_t new_pos = { .x = p_snake_head->pos.x + p_snake_head->dir.x,
                        .y = p_snake_head->pos.y + p_snake_head->dir.y };
    // check if snake head is colliding within itself
    if (sll_is_in(p_game->p_snake, (const void *)&(new_pos)))
    {
        goto EXIT;
    }

    entity_t * p_snake_tail = (entity_t *)p_game->p_snake->p_head->p_data;
    // p_head is the tail of the snake
    game_place_tile(p_game, p_snake_tail->pos, EMPTY);

    sll_node_t * p_temp_node = p_game->p_snake->p_head;

    while (NULL != p_temp_node)
    {
        // the section before the head
        if (NULL != p_temp_node->p_next)
        {
            ((entity_t *)p_temp_node->p_data)->dir.x
                = ((entity_t *)p_temp_node->p_next->p_data)->dir.x;
            ((entity_t *)p_temp_node->p_data)->dir.y
                = ((entity_t *)p_temp_node->p_next->p_data)->dir.y;
            ((entity_t *)p_temp_node->p_data)->pos.x
                = ((entity_t *)p_temp_node->p_next->p_data)->pos.x;
            ((entity_t *)p_temp_node->p_data)->pos.y
                = ((entity_t *)p_temp_node->p_next->p_data)->pos.y;
        }

        p_temp_node = p_temp_node->p_next;
    }

    p_snake_head->pos.x += p_snake_head->dir.x;
    p_snake_head->pos.y += p_snake_head->dir.y;

    if (0 > p_snake_head->pos.x || p_game->game_size <= p_snake_head->pos.x
        || 0 > p_snake_head->pos.y || p_game->game_size <= p_snake_head->pos.y)
    {
        // gb_run = false;
        goto EXIT;
    }

    for (size_t entity_idx = 0; entity_idx < p_game->p_entity_arr->size;
         entity_idx++)
    {
        entity_t * p_entity
            = (entity_t *)dyn_arr_get(p_game->p_entity_arr, entity_idx);

        if (p_snake_head->pos.x == p_entity->pos.x
            && p_snake_head->pos.y == p_entity->pos.y)
        {
            p_entity->is_deletable = true;
            p_game->score += p_entity->score;

            if (FOOD == p_entity->entity_type)
            {
                point_t pos = { .x = rand() % p_game->game_size,
                                .y = rand() % p_game->game_size };
                game_add_entity(p_game, pos, g_zero, FOOD);
                game_place_tile(p_game, pos, FOOD);
            }

            game_add_entity(p_game, p_snake_tail->pos, g_zero, PLAYER);
            game_place_tile(p_game, p_snake_tail->pos, PLAYER);
            break;
        }
    }

    game_place_tile(p_game, p_snake_head->pos, PLAYER);

    should_update = true;
EXIT:
    return (should_update);
}

/*** end of file ***/
