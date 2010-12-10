#include <sys/time.h>

/********************************************//**
 * \struct Node
 *
 * A detail description
 ***********************************************/
typedef struct Node
{
  /**A pointer to the actual data*/
  void *data; 
  /**A pointer to next node*/
  struct Node *next;
  /**A pointer to previous node*/
  struct Node *prev;
}Node;


typedef struct List
{
  /**A pointer to the first node in the list*/
  Node *first_node;
  /**A pointer to the last node in the list*/
  Node *last_node;
}List;

typedef struct Card
{
  /**A pointer to a string of card entry*/
  char *entry;
  /**time need to be delayed for next review in seconds*/
  float delay;
  /**time to be reviewed in seconds from 1970-01-01*/
  time_t review_time;
}Card;


