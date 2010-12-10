#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "misc.h"

void
init_config_structure ()
{
  char conf_dir[2000] ;
  char conf_users[2000];
  
  strcpy (conf_dir, getenv("HOME"));
  strcat (conf_dir, "/.voboo");
  strcpy (conf_users, conf_dir);
  strcat (conf_users, "/user_list");
 
  if(errno != 2)
  {
    nosave_quit ();
    error (1, errno, "error in reading %s", conf_dir);
  }
  
  if(mkdir (conf_dir, 0777))
  {
    nosave_quit ();
    error (1, errno, "can't create %s", conf_dir);
  }

  if(creat (conf_users, 0777) == -1)
  {
    nosave_quit ();
    error (1, errno, "can't create %s", conf_users);
  }
 
  char user_dir[2000];
  strcpy (user_dir, conf_dir);
  strcat (user_dir, "/users");
  if(mkdir (user_dir, 0777))
  {
    nosave_quit ();
    error (1, errno, "can't create %s", user_dir);
  }

  char i,j,k;
  char repo_dir_1[2000];
  char repo_dir_2[2000];
  char repo_dir_3[2000];
  char suffix[4];
  strcat (conf_dir, "/cache");
  if(mkdir (conf_dir, 0777))
  {
    save_for_quit ();
    error (1, errno, "can't create %s", conf_dir);
  }
  for(i='`'; i<='z'; i++)
  { 
    strcpy (repo_dir_1, conf_dir);
    suffix[0]=(i=='`')?'_':i;
    suffix[1]=0;
    strcat (repo_dir_1, "/");
    strcat (repo_dir_1, suffix);
    if(mkdir (repo_dir_1, 0777))
    {
      save_for_quit ();
      error (1, errno, "can't create %s", repo_dir_1);
    }
    for(j='`'; j<='z'; j++)
    {
      strcpy (repo_dir_2, repo_dir_1);
      suffix[1]=(j=='`')?'_':j;
      suffix[2]=0;
      strcat (repo_dir_2, "/");
      strcat (repo_dir_2, suffix);
      if(mkdir (repo_dir_2, 0777))
      {
     	  save_for_quit ();
    	  error (1, errno, "can't create %s", repo_dir_2);
    	}
      for(k='`'; k<='z'; k++)
      {
        strcpy (repo_dir_3, repo_dir_2);
        suffix[2]=(k=='`')?'_':k;
        suffix[3]=0;
        strcat (repo_dir_3, "/");
        strcat (repo_dir_3, suffix);
        if(mkdir (repo_dir_3, 0777))
        {
     	    save_for_quit ();
    	    error (1, errno, "can't create %s", repo_dir_3);
    	  }
      }
    }
  }
}

void
check_config (void)
{
  struct stat dir_stat;

  char conf_dir[2000] ;
  
  strcpy (conf_dir, getenv("HOME"));
  strcat (conf_dir, "/.voboo");
  
  
  if(stat(conf_dir, &dir_stat))
  {
    init_config_structure ();
  }
}


