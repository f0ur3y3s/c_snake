#include "../include/main.h"

static _Atomic bool gb_run = true;

int main (void)
{
    int status = term_uncook();

    if (0 != status)
    {
        goto EXIT;
    }

    // game_t * p_game = game_init(BOARD_SIZE);
    game_t * p_game = game_init(20);


    if (NULL == p_game)
    {
        goto COOK_EXIT;
    }

    system("clear");
    game_print_tiles(p_game);

    for (;;)
    {
        if (game_tick(p_game))
        {
            game_print_tiles(p_game);
        }

        char chr[3] = { 0 };

        ssize_t bytes_read = 0;

        bytes_read = read(STDIN_FILENO, &chr, 3);

        if (0 > bytes_read)
            continue;

        if (3 == chr[0])
        {
            printf("Exiting...\n");
            break;
        }

        point_t xy_delta = { 0 };

        switch (chr[0])
        {
            case MOVEMENT_KEY_UP:
                xy_delta.y = -1;
                break;
            case MOVEMENT_KEY_DOWN:
                xy_delta.y = 1;
                break;
            case MOVEMENT_KEY_RIGHT:
                xy_delta.x = 1;
                break;
            case MOVEMENT_KEY_LEFT:
                xy_delta.x = -1;
                break;
            default:
                break;
        }

        if (xy_delta.x != 0 || xy_delta.y != 0)
        {
            game_turn_player(p_game, xy_delta);
            game_print_tiles(p_game);
        }
    }

    game_destroy(&p_game);

COOK_EXIT:
    status = term_cook();
EXIT:
    return status;
}