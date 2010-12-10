/********************************************//**
 * \file list.c
 *
 * A series of function to manipulate doubly-
 * linked lists
 ***********************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <time.h>

#include "types.h"

/********************************************//**
 *Insert [new_node] after [node] in [list]
 *
 *@param[in,out] list     The list that [new_node] to be inserted to
 *@param[in] node         The node after which [new_node] to be inserted
 *@param[in] new_node     The new node to be inserted
 ***********************************************/
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

/********************************************//**
 *Insert [new_node] before [node] in [list]
 *
 *@param[in,out] list     The list that [new_node] to be inserted to
 *@param[in] node         The node before which [new_node] to be inserted
 *@param[in] new_node     The new node to be inserted
 ***********************************************/
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

/********************************************//**
 *Insert [new_node] at the beginning of [list]
 *
 *@param[in,out] list     The list that [new_node] to be inserted to
 *@param[in] new_node     The new node to be inserted at the beginning of [list]
 ***********************************************/
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

/********************************************//**
 *Insert [new_node] at the end of [list]
 *
 *@param[in,out] list     The list that [new_node] to be inserted to
 *@param[in] new_node     The new node to be inserted at the beginning of [list]
 ***********************************************/
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

/********************************************//**
 *Create [list] for [username] as a [listname] TODO Not to be related to card
 *
 *@param[in] username     The username for who [list] to be created for
 *@param[in] listname     The listname (newlist or reviewlist) belongs to [list]
 *@param[out] list        The new created list for output
 ***********************************************/
void
create_list (char *username, char *listname, List *list)
{
  char list_file[2000];
  char buffer[2000];
  
  strcpy (list_file, getenv("HOME"));
  strcat (list_file, "/.voboo/users/");
  strcat (list_file, username);
  strcat (list_file, "/");
  strcat (list_file, listname);

  FILE *fp = fopen (list_file, "r");
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
    
    list_insert_end (list, node);
  }
  fclose (fp);  
}

/********************************************//**
 *Create all lists (newlist and reviewlist) for [username]
 *
 *@param[in] username     The username for who lists to be created for
 *@param[out] newlist     The new word list to be created
 *@param[out] reviewlist  The review word list to be created
 ***********************************************/
void
create_all_list (char *username, List *newlist, List *reviewlist)
{
  create_list (username, "newlist", newlist);
  create_list (username, "reviewlist", reviewlist);
}

/********************************************//**
 *Release all memory of a list
 *
 *@param[in,out] list     The list to be released
 ***********************************************/
void
release_list (List *list)
{
  //TODO
}

/********************************************//**
 *Release all memory of all lists (newlist and reviewlist)
 *
 *@param[in,out] newlist     The new word list to be released
 ***********************************************/
void
release_all_list (List *newlist, List *reviewlist)
{
  //TODO
}

/********************************************//**
 * Get the number of nodes in [list]
 *
 * @param[in] list The list whose items need to be counted
 * @return    number of items in the list
 ***********************************************/
int 
list_get_size (List *list)
{
  int n = 0;
  Node *node = list->first_node;
  while(node)
  {
    n++;
    node=node->next;
  }

  return n;
}

/********************************************//**
 * Rearrange nodes in [list] randomly using Arnoldi algorithm
 *
 * @param[in] list The list whose items need to be shuffled
 ***********************************************/
void
shuffle_list(List *list)
{
  int i,j;
  int n = list_get_size (list);

  Node *node = list->first_node;
  Node **node_ptr = malloc (n*sizeof(Node*));
  
  for (i=0; i<n; i++)
  {
    node_ptr[i]=node;
    node=node->next;
  }

  for (i=n-1; i>1; i--)
  {
    j=rand()%i; //FIXME:srand()
    Node *tmp;

    tmp = node_ptr[i]->data;
    node_ptr[i]->data = node_ptr[j]->data;
    node_ptr[j]->data = tmp;
  }
  
  free (node_ptr);
  node_ptr = NULL;
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


