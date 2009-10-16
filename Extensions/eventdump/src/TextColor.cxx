#include "TextColor.hh"


void 
reset_color()
{
  printf("\033[0m");
}


void 
text_color(int fg)
{
  char command[13];
  sprintf(command, "\033[%dm", fg+30);
  printf("%s", command);
}


void 
text_attribute(int att)
{
  char command[13];
  sprintf(command, "\033[%dm", att);
  printf("%s", command);
}
