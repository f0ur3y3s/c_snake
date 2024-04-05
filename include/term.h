#ifndef TERM_H
#define TERM_H

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/**
 * @brief Sets the terminal to raw mode and sets to non-blocking mode.
 *
 * @note For this function to perform correctly, your terminal must be in a
 * "good" state before calling this function. This function will not restore
 * the terminal to its original state if the original state was bungled.
 *
 * @return int
 * @retval 0 Success
 * @retval -1 Error
 */
int term_uncook (void);

/**
 * @brief Restores the terminal to the state before term_uncook was called.
 *
 * @return int
 * @retval 0 Success
 * @retval -1 Error
 */
int term_cook (void);

void term_clear (void);

// starts at 1 for both x and y
void term_gotoxy (int x, int y);

void term_hide_cursor(void);

void term_show_cursor(void);

#endif // TERM_H

/*** end of file ***/