#ifndef TERM_H
#define TERM_H

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int term_uncook (void);
int term_cook (void);

#endif // TERM_H

/*** end of file ***/