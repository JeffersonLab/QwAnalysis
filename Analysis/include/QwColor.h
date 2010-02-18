#ifndef QWCOLOR_H
#define QWCOLOR_H

// This header contains the terminals color codes.
// This might be useful to distingush many output of QwAnalysis into
// terminals.


// Simply use printf function
// printf("%s Bold text print  %s\n", BOLD, NORMAL);
// printf("%s RED text %s\n", RED, NORMAL);

// detailed information 
// see http://www.faqs.org/docs/abs/HTML/colorizing.html
//     http://linuxgazette.net/issue65/padala.html

#define RED       "\E[31m"
#define GREEN     "\E[32m"
#define BROWN     "\E[33m"
#define BLUE      "\E[34m"
#define MAGENTA   "\E[35m"
#define CYAN      "\E[36m"
#define WHITE     "\E[37m"

#define BOLD        "\033[1m"

#define BOLDRED     "\E[31m\033[1m"
#define BOLDGREEN   "\E[32m\033[1m"
#define BOLDBROWN   "\E[33m\033[1m"
#define BOLDBLUE    "\E[34m\033[1m"
#define BOLDMAGENTA "\E[35m\033[1m"
#define BOLDCYAN    "\E[36m\033[1m"
#define BOLDWHITE    "\E[37m\033[1m"

#define BACKRED     "\E[31m\033[7m"
#define BACKGREEN   "\E[32m\033[7m"
#define BACKBLUE    "\E[34m\033[7m"
#define NORMAL      "\033[0m"

#include <cstdio>

#endif
