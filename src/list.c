#include <stdlib.h>
#include "types.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
  strcat (newlist_file, "/.voboo/users/");
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
    
    char *str = malloc (strlen (tmp_ch) * sizeof (char)+1);
    str = strcpy(str, tmp_ch);
    
    Card *card;
    if (!(card = malloc (sizeof (Card))))
    {
	error(1, errno, "Error when allocating memory");
    }
    card->entry = str;
    card->delay = tmp_fl;
    card->review_time = tmp_tm;
    
    Node *node;
    if (!(node = malloc (sizeof (Node))))
    {
	error(1, errno, "Error when allocating memory");
    }
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
  strcat(user_dir, "/.voboo/users/");
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


