#ifndef TextColor_hh
#define TextColor_hh

// the reference is http://linuxgazette.net/issue65/padala.html
#include <stdio.h>
#include <stdlib.h>

#define RESET		0
#define BOLD 		1
#define UNDERLINE	4
#define BLINK		5
#define REVERSE		7
#define HIDDEN          8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

//void textcolor(int attr, int fg, int bg);
void text_color    (int fg);
void text_attribute(int att);
void reset_color   ();
#endif
        


