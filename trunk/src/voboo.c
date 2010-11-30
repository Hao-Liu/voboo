#include <iconv.h>
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <openssl/md5.h>

void
get_md5 (char *password, char *md5out)
{
  MD5_CTX md5;
  unsigned char digest[16];
  int i;
  
  MD5_Init (&md5);
  MD5_Update (&md5, password, strlen(password));
  MD5_Final (digest, &md5);
  for (i=0; i<16; i++)
  {
     sprintf(md5out+2*i, "%02X", digest[i]);
  }
  md5out[32] = 0;
}

int
username_exist(char *username, char *passmd5)
{
  char file_name[2000];
  strcpy (file_name, getenv("HOME"));
  strcat (file_name, "/.voboo/users");

  char buffer[2000];
  char uname_tmp[2000];
  char md5_tmp[2000];
  FILE *fp = fopen (file_name, "r");
  
  //FIXME username need to be checked for illegle characters
  while(fgets (buffer, 2000, fp))
  {
    sscanf (buffer, "%s %s", uname_tmp, md5_tmp);
    if (!strcmp(uname_tmp, username))
    {
      strcpy(passmd5, md5_tmp);
      return 1;
    }
  }
  
  fclose (fp);
  return 0;
}

void
add_user(char *username, char *passmd5)
{
  char file_name[2000];
  strcpy (file_name, getenv("HOME"));
  strcat (file_name, "/.voboo/users");
  
  char buffer[2000];
  char uname_tmp[2000];
  FILE *fp = fopen (file_name, "a");

  fprintf(fp, "%s %s\n", username, passmd5);
  
  fclose (fp);
}

void
init_user (char *username, char *dictfile)
{
  return;
}

void
delete_window (WINDOW *win)
{
  wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  wclear (win);
  wrefresh (win);
  delwin (win);
}
void
choose_dict (char *username)
{
  WINDOW *win_dict;
  int row, col;

  getmaxyx (stdscr, row, col);

  int height = 10;
  int width = 25;
  win_dict = newwin (height, width, (row-height)/2, (col-width)/2);
  box (win_dict, 0, 0);
  wrefresh (win_dict);
  wmove (win_dict, 1, 1);
  wprintw (win_dict, "Select a Vocabulary:");

  wmove (win_dict, 2, 1);
  wprintw (win_dict, "1. GRE");

  wmove (win_dict, 3, 1);
  wprintw (win_dict, "2. TOEFL");

  char dict[2000];
  noecho ();
  char ch = wgetch (win_dict);
  echo ();
  switch (ch)
  {
    case '1':
      strcpy(dict, "gre");
      break;
    case '2':
      strcpy(dict, "toefl");
      break;
    default:
      delwin(win_dict);
      choose_dict (username);
  }
  
  char dict_file[2000];
  char dict_dir[2000]="/usr/share/voboo/dict/";
  char user_dir[2000];
  char newlist_file[2000];
  char reviewlist_file[2000];
  char buffer[2000];
  strcpy(dict_file, dict_dir);
  strcat(dict_file, dict);

  FILE *fp_dict = fopen (dict_file, "r");
  
  strcpy(user_dir, getenv("HOME"));
  strcat(user_dir, "/.voboo/");
  strcat(user_dir, username);
  if(mkdir (user_dir, 0777) == -1)
  {
    endwin ();
    error (1, errno, "can't create %s", user_dir);
  }
  else
  {
    strcpy(newlist_file, user_dir);
    strcat(newlist_file, "/newlist");
    if(creat (newlist_file, 0777) == -1)
    {
      endwin ();
      error (1, errno, "can't create %s", newlist_file);
    }
    

    strcpy(reviewlist_file, user_dir);
    strcat(reviewlist_file, "/reviewlist");
    if(creat (reviewlist_file, 0777) == -1)
    {
      endwin ();
      error (1, errno, "can't create %s", reviewlist_file);
    }
  }

  FILE *fp_newlist = fopen (newlist_file, "w");
  while (fgets (buffer, 2000, fp_dict))
  {
    buffer[strlen(buffer)-1] = 0;
    fprintf (fp_newlist, "%s   0.0   0\n", buffer);
  }
  fclose (fp_newlist);
  fclose (fp_dict);
  delete_window (win_dict);
}

int
authenticate(char *username)
{
  WINDOW *win_auth;
  int row, col;
  char passmd5[2000];
  char inputmd5[2000];
  char dictfile[2000];

  getmaxyx (stdscr, row, col);
  
  int height = 7;
  int width = 30;
  win_auth = newwin (height, width, (row-height)/2, (col-width)/2);
  box (win_auth, 0, 0);
  wrefresh (win_auth);

  char txt_title[] = "Authorization";
  wmove (win_auth, 1, (width - sizeof (txt_title)) / 2);
  wprintw (win_auth, txt_title);
  
  char txt_username[] = "User Name:";
  wmove (win_auth, 3, 2);
  wprintw (win_auth, txt_username);
  
  char txt_password[] = "Password :";
  wmove (win_auth, 4, 2);
  wprintw (win_auth, txt_password);
  
  wmove (win_auth, 3, sizeof (txt_username)+1);
  wscanw (win_auth, "%s", username);
  
  if(username_exist(username, passmd5))
  {
    char password[2000];
    wmove (win_auth, 4, sizeof (txt_password)+1);
    noecho ();
    wscanw (win_auth, "%s", password);
    echo ();
    get_md5 (password, inputmd5);
    if(strcmp (passmd5, inputmd5))
    {
      authenticate(username);
    }
    else
    {
      delete_window (win_auth);
      return 1;
    }
  }
  else
  {
    char txt_retype[] = "Retype it:";
    wmove (win_auth, 5, 2);
    wprintw (win_auth, txt_retype);
    
    char password[2000];
    wmove (win_auth, 4, sizeof (txt_password)+1);
    noecho ();
    wscanw (win_auth, "%s", password);
    echo ();

    char retype[2000];
    wmove (win_auth, 5, sizeof (txt_retype)+1);
    noecho ();
    wscanw (win_auth, "%s", retype);
    echo ();
    
    if (strcmp (password, retype))
    {
      authenticate (username);
    }
    else
    {
      get_md5 (password, inputmd5);
      add_user(username, inputmd5);

      choose_dict (username);
      delete_window (win_auth);
      return 1;
    }
  }
}

void
check_config (void)
{
  struct stat dir_stat;

  char conf_dir[2000] ;
  char conf_users[2000];
  
  strcpy (conf_dir, getenv("HOME"));
  strcat (conf_dir, "/.voboo");
  strcpy (conf_users, conf_dir);
  strcat (conf_users, "/users");
  
  refresh();
  
  if(stat(conf_dir, &dir_stat))
  {
    if(errno != 2)
    {
      endwin ();
      error (1, errno, "error in reading %s", conf_dir);
    }
    
    if(mkdir (conf_dir, 0777))
    {
      endwin ();
      error (1, errno, "can't create %s", conf_dir);
    }
    
  }

  if(stat(conf_users, &dir_stat))
  {
    if(errno != 2)
    {
      endwin ();
      error (1, errno, "error in reading %s", conf_users);
    }

    if(creat (conf_users, 0777) == -1)
    {
      endwin ();
      error (1, errno, "can't create %s", conf_users);
    }
  }
}

typedef struct Node
{
  void *data;
  struct Node *next;
  struct Node *prev;
}Node;


typedef struct List
{
  Node *first_node;
  Node *last_node;
}List;

typedef struct Card
{
  char *entry;
  float delay;
  time_t review_time;
}Card;

void
list_insert_after (List *list, Node *node, Node *new_node)
{
  new_node->prev = node;
  new_node->next = node->next;
  if (node->next)
  {
    node->next->prev = new_node;
  }
  else
  {
    list->last_node = new_node;
  }
  node->next = new_node;
}

void
list_insert_before (List *list, Node *node, Node *new_node)
{
  new_node->prev = node->prev;
  new_node->next = node;
  if (node->prev)
  {
    node->prev->next = new_node;
  }
  else
  {
    list->first_node = new_node;
  }
  node->prev = new_node;
}

void
list_insert_beginning (List *list, Node *new_node)
{
  if (list->first_node)
  {
    list_insert_before (list, list->first_node, new_node);
  }
  else
  {
    list->first_node = new_node;
    list->last_node = new_node;
    new_node->prev = NULL;
    new_node->next = NULL;
  }
}

void
list_insert_end (List *list, Node *new_node)
{
  if (list->last_node)
  {
    list_insert_after (list, list->last_node, new_node);
  }
  else
  {
    list_insert_beginning (list, new_node);
  }
}

void
create_list (char *username, char *listname, List *newlist)
{
  char newlist_file[2000];
  char buffer[2000];
  
  strcpy (newlist_file, getenv("HOME"));
  strcat (newlist_file, "/.voboo/");
  strcat (newlist_file, username);
  strcat (newlist_file, "/");
  strcat (newlist_file, listname);

  FILE *fp = fopen (newlist_file, "r");
  while (fgets (buffer, 2000, fp))
  {
    char tmp_ch[2000];
    float tmp_fl;
    time_t tmp_tm;
    sscanf(buffer, "%s %f %ld", tmp_ch, &tmp_fl, &tmp_tm);
    
    char *str = malloc (strlen (tmp_ch) * sizeof (char));
    str = strcpy(str, tmp_ch);
    
    Card *card = malloc (sizeof (Card));
    card->entry = str;
    card->delay = tmp_fl;
    card->review_time = tmp_tm;
    
    Node *node = malloc (sizeof (Node));
    node->prev = NULL;
    node->next = NULL;
    node->data = card;
    
    list_insert_end (newlist, node);
  }
  fclose (fp);  
}

void
create_all_list (char *username, List *newlist, List *reviewlist)
{
  create_list (username, "newlist", newlist);
  create_list (username, "reviewlist", reviewlist);
}
void
release_newlist (List *newlist)
{
  //TODO
}

void
release_reviewlist (List *reviewlist)
{
  //TODO
}

Card *
get_first_card (List *list)
{
  Node *node;
  Card *card;

  node = list->first_node;
  if(node->next)
    node->next->prev = NULL;
  list->first_node = node->next;
  
  node->prev = NULL;
  node->next = NULL;
  
  card = node->data;
  free (node);
  node = NULL;
  return card;
}

Card *
select_card (List *newlist, List *reviewlist)
{
  if (reviewlist->first_node)
  {
    Card *card = reviewlist->first_node->data;
    if (card->review_time < time(NULL))
    {
      return get_first_card (reviewlist);
    }
    else
    {
      return get_first_card (newlist);
    }
  }
  else
  {
    return get_first_card (newlist);
  }
}

void save_quit ()
{
  exit (0);
}

int
show_question (Card *card)
{
  WINDOW *win_quest;
  int row, col;
  getmaxyx (stdscr, row, col);
  
  int height = 10;
  int width = 40;
//  keypad (win_quest, TRUE);
  win_quest = newwin (height, width, (row-height)/2, (col-width)/2);

	attron(COLOR_PAIR(2));
	wmove (win_quest, 3, (width - strlen (card->entry)) / 2);
  wprintw (win_quest, "%s", card->entry);
  attroff(COLOR_PAIR(2));  
  
  wmove (win_quest, 6, (width - strlen ("Do you know this word?")) / 2);
  wprintw (win_quest, "Do you know this word?");
  
  wmove (win_quest, 8, (width - strlen ("<- YES       NO ->")) / 2);
  wprintw (win_quest, "<- YES       NO ->");
  
  
  box (win_quest, 0, 0);
  wrefresh (win_quest);

  noecho ();
  while (1)
  {
    switch (getch ())
    {
      case KEY_LEFT:
        echo ();
        delete_window (win_quest);
        return 1;
      case KEY_RIGHT:
        echo ();
        delete_window (win_quest);
        return 0;
      case 'x':
        delete_window (win_quest);
        save_quit(0); //FIXME
        return 0;
      default:
        wrefresh (win_quest);
    }
  }
}

void
play_sound (Card *card)
{
  char cmd[2000]="mplayer `wget -q http://www.google.com/dictionary?langpair=en\\|zh-CN\\&q=";
  strcat(cmd, card->entry);
  strcat(cmd, " -O - | grep -m 1 -e 'http://www.gstatic.com/[a-zA-Z0-9/%%_]*.mp3' -o` 2>/dev/null > /dev/null");
  system(cmd);
}

void
get_translation (Card *card)
{
  char cmd[2000]="wget -q http://www.google.com/dictionary?langpair=en\\|zh-CN\\&q=";
  strcat(cmd, card->entry);
  strcat(cmd, " -O - | grep -e '<div  class=\"dct-em\">' -A 1 ");
  strcat(cmd, "| grep -e '<span class=\"dct-tt\">[^a-zA-Z]*' ");
  strcat(cmd, "| sed -e 's/<span class=\"dct-tt\">[a-zA-Z0-9 ;,.\\/()'\\''‘’?!`~@#$%^&*=_+{}<>-]*<\\/span>//g' ");
  strcat(cmd, "| sed -e 's/<span class=\"dct-tt\">//g' ");
  strcat(cmd, "| sed -e 's/<\\/span>//g'");
  //wget -q http://www.google.com/dictionary?langpair=en\|zh-CN\&q=<<<entry>>> -O - 
  //| grep -e '<div  class="dct-em">' -A 1 
  //| grep -e '<span class="dct-tt">[^a-zA-Z]*' 
  //| sed -e 's/<span class="dct-tt">[a-zA-Z ;,.\/()'\''‘’?!`~@#$%^&*-=_+{}<>]*<\/span>//g' 
  //| sed -e 's/<span class="dct-tt">//g' 
  //| sed -e 's/<\/span>//g'
  char buffer[2000]={0};
  
  FILE *fp = fopen ("", "w");

  FILE *stream = popen (cmd, "r");
  while(fgets (buffer, 2000, stream))
  {
    fprintf (fp, "%s", buffer);
  }
  pclose (stream);
}

void
show_translation (WINDOW* win, Card *card)
{
  char cmd[2000]="wget -q http://www.google.com/dictionary?langpair=en\\|zh-CN\\&q=";
  strcat(cmd, card->entry);
  strcat(cmd, " -O - | grep -e '<div  class=\"dct-em\">' -A 1 ");
  strcat(cmd, "| grep -e '<span class=\"dct-tt\">[^a-zA-Z]*' ");
  strcat(cmd, "| sed -e 's/<span class=\"dct-tt\">[a-zA-Z0-9 ;,.\\/()'\\''‘’?!`~@#$%^&*=_+{}<>-]*<\\/span>//g' ");
  strcat(cmd, "| sed -e 's/<span class=\"dct-tt\">//g' ");
  strcat(cmd, "| sed -e 's/<\\/span>//g'");
  //wget -q http://www.google.com/dictionary?langpair=en\|zh-CN\&q=<<<entry>>> -O - 
  //| grep -e '<div  class="dct-em">' -A 1 
  //| grep -e '<span class="dct-tt">[^a-zA-Z]*' 
  //| sed -e 's/<span class="dct-tt">[a-zA-Z ;,.\/()'\''‘’?!`~@#$%^&*-=_+{}<>]*<\/span>//g' 
  //| sed -e 's/<span class="dct-tt">//g' 
  //| sed -e 's/<\/span>//g'
  char buffer[2000]={0};

  FILE *stream = popen (cmd, "r");
  while(fgets (buffer, 2000, stream))
  {
    wprintw (win, buffer);
  }
  pclose (stream);
}

int
show_answer (int know, Card *card)
{
  WINDOW *win_answer;
  int row, col;
  getmaxyx (stdscr, row, col);
  
  pid_t pid;
  
  int height = 30;
  int width = 60;

  win_answer = newwin (height, width, (row-height)/2, (col-width)/2);

  if(know)
  {
    wmove (win_answer, height-6, (width - strlen ("Is your answer correct?")) / 2);
    wprintw (win_answer, "Is your answer correct?");
    
    wmove (win_answer, height-4, (width - strlen ("<- YES   V MAYBE   NO ->")) / 2);
    wprintw (win_answer, "<- YES   V MAYBE   NO ->");
  }
  
	wmove (win_answer, 2, (width - strlen (card->entry)) / 2);
  wprintw (win_answer, "%s", card->entry);
  
	wmove (win_answer, 4, 2);
  wprintw (win_answer, "Loading...");
  
  box (win_answer, 0, 0);
  wrefresh (win_answer);

	wmove (win_answer, 4, 2);
	show_translation (win_answer, card);

  box (win_answer, 0, 0);
  wrefresh (win_answer);
  
  pid = fork();
  
  if(!pid)
  {
    play_sound (card);
    exit (0);
  }
  noecho ();
  while (1)
  {
    switch (getch ())
    {
      case KEY_LEFT:
        echo ();
        delete_window (win_answer);
        return know?2:0;
      case KEY_DOWN:
        echo ();
        delete_window (win_answer);
        return know?1:0;
      case KEY_RIGHT:
        echo ();
        delete_window (win_answer);
        return 0;
      case 'x':
        delete_window (win_answer);
        save_quit(0);
        return 0;
      default:
        wrefresh (win_answer);
    }
  }
}

int
get_delay (int assess, float time_used, Card *card)
{
  float param1 = 1.0f;
  float param2 = 2.5f;
  float param3 = 5.0f;
  time_t current_time;
//  gettimeofday( &current_time, NULL);
  current_time = time (NULL);
  
  if(card->delay == 0.0f)
  {
    switch(assess)
    {
      case 0:
        card->delay = 10.0f;
        break;
      case 1:
        card->delay = 1000.0f;
        break;
      case 2:
        card->delay = 100000.0f;
        break;
      default:
        break;
    }
  }
  else
  {
    
    switch(assess)
    {
      case 0:
        card->delay = (card->delay + (current_time-card->review_time)/4.0f)*param1;
        break;
      case 1:
        card->delay = (card->delay + (current_time-card->review_time)/2.0f)*param2;
        break;
      case 2:
        card->delay = (card->delay + (current_time-card->review_time))*param3;
        break;
      default:
        break;
    }
  }
  card->review_time = current_time + (long)card->delay;
}

void
insert_card (Card* card, List* list)
{
  Node *node = list->first_node;
  Node *new_node = malloc (sizeof (Node));
  new_node->data = card;
  if (node)
  {
    while (1)
    {
      if (card->review_time >= ((Card *)(node->data))->review_time) 
      {
        if (node->next)
        {
          node = node->next;
          continue;
        }
        else
        {
          list_insert_after (list, node, new_node);
          break;
        }
      }
      else
      {
        list_insert_before (list, node, new_node);
        break;
      }
    }
  }
  else
  {
    list_insert_beginning (list, new_node);
  }
}

WINDOW *win_debug; //FIXME DEBUG

//FIXME DEBUG
void 
create_debug ()
{
  int row, col;
  getmaxyx (stdscr, row, col);
  
  int height = 30;
  int width = 30;

  win_debug = newwin (height, width, 1,1);

  box (win_debug, 0, 0);
  wrefresh (win_debug);
}

//FIXME DEBUG
void 
debug (const char *str, ...)
{
  wmove (win_debug, 1, 1);
  wprintw (win_debug, str);
}

//FIXME DEBUG
void
print_list (List *list)
{
  Node *node = list->first_node;

  time_t current_time;
  current_time = time (NULL);

  wmove (win_debug, 1, 1);
  while (node)
  {
    Card *card = node->data;
    wprintw (win_debug, " %s %f %ld\n", card->entry, card->delay, card->review_time-current_time);
    node = node->next;
  }
  box (win_debug, 0, 0);
  wrefresh (win_debug);
}

void
save_list (FILE* fp, List *list)
{
  Node *node = list->first_node;
  while(node)
  {
    Card* card = node->data;
    fprintf(fp, "%s %f %ld\n", card->entry, card->delay, card->review_time);
    node = node->next;
  }
}

void 
save_all_list (char * username, List *newlist, List *reviewlist)
{
  char user_dir[2000];
  char newlist_file[2000];
  char reviewlist_file[2000];
  char buffer[2000];
  
  strcpy(user_dir, getenv("HOME"));
  strcat(user_dir, "/.voboo/");
  strcat(user_dir, username);
  strcpy(newlist_file, user_dir);
  strcat(newlist_file, "/newlist");
  strcpy(reviewlist_file, user_dir);
  strcat(reviewlist_file, "/reviewlist");

  FILE *newlist_fp = fopen (newlist_file, "w");
  FILE *reviewlist_fp = fopen (reviewlist_file, "w");
  
  save_list (newlist_fp, newlist);
  save_list (reviewlist_fp, reviewlist);
  
  fclose (newlist_fp);
  fclose (reviewlist_fp);
}

int
main (int argc, char *argv[])
{
  int done = FALSE;
  int know = FALSE;
  int assess = 0;
  struct timeval start, end;
  float  time_used = 0.0f;
  
  char username[2000];
  List newlist;
  newlist.first_node = NULL;
  newlist.last_node = NULL;
  
  List reviewlist;
  reviewlist.first_node = NULL;
  reviewlist.last_node = NULL;
  
  setlocale(LC_ALL,"");
  initscr ();
  cbreak ();
  keypad (stdscr, TRUE);

  check_config ();
  if(authenticate (username))
  {
    create_all_list (username, &newlist, &reviewlist);
    while (!done)
    {
      Card *card = select_card (&newlist, &reviewlist);
      gettimeofday (&start, NULL);
      know = show_question (card);
      gettimeofday (&end, NULL);
      time_used = (float)(end.tv_sec - start.tv_sec) + (float)(end.tv_usec-start.tv_usec)*1e-6;
      assess = show_answer (know, card);
      get_delay (assess, time_used, card);
      insert_card (card, &reviewlist);
      save_all_list (username, &newlist, &reviewlist);
    }
  }

  release_newlist (&newlist);
  release_reviewlist (&reviewlist);
  endwin ();
  return 0;
}
