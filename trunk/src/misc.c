#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

void
fork_me ()
{
  pid_t pid;
  if ( (pid=fork()) < 0)
  {
    error (0, errno, "can't fork");
  }
  else
  {
    if(pid == 0) //son
    {
      if ( (pid=fork()) < 0)
      {
        error (0, errno, "can't fork");
      }
      else
      {
        if (pid > 0) //son
        {
          exit (0); //suicide
        }
        else //grandson
        {
          //TODO insert your code here;
          while (getppid() != 1) //if my dad is dead and I'm adopted by YKW
          {
            sleep(1); //sleeping
          }
          exit(0); //see you in the underworld, dad
        }
      }
    }
    if (waitpid (pid, NULL, 0) != pid)
    {
      error (0, errno, "this is not my son");
    }
  }
}

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


