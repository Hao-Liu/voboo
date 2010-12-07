#include <sys/time.h>
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


