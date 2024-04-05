#include "../include/term.h"

struct termios g_cooked = { 0 };

int term_uncook (void)
{
    int            status = -1;
    struct termios t_raw  = { 0 };

    if (0 > tcgetattr(STDIN_FILENO, &g_cooked))
    {
        perror("tcgetattr()");
        goto EXIT;
    }

    t_raw = g_cooked;
    t_raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    t_raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    t_raw.c_cflag |= CS8;
    t_raw.c_cc[VMIN]  = 1;
    t_raw.c_cc[VTIME] = 0;

    if (0 > tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_raw))
    {
        perror("tcsetattr()");
        goto EXIT;
    }

    status    = 0;
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

EXIT:
    return (status);
}

int term_cook (void)
{
    int status = -1;

    if (0 > tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_cooked))
    {
        perror("tcsetattr()");
        goto EXIT;
    }

    // set stdin back to blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

    status = 0;

EXIT:
    return (status);
}

/*** end of file ***/