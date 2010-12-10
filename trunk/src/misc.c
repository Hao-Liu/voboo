#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

void 
save_quit ()
{
  kill (0, SIGINT); //SIGKILL
  endwin ();
  exit (0);
}

void 
save_for_quit ()
{
  kill (0, SIGINT); //SIGKILL
  endwin ();
}

void 
nosave_quit ()
{
  kill (0, SIGINT); //SIGKILL
  endwin ();
  exit (0);
}


