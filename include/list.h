#ifndef _VB_LIST_H_
#define _VB_LIST_H_

void
list_insert_after (List *list, Node *node, Node *new_node);
void
list_insert_before (List *list, Node *node, Node *new_node);
void
list_insert_beginning (List *list, Node *new_node);
void
list_insert_end (List *list, Node *new_node);
void
create_list (char *username, char *listname, List *newlist);
void
create_all_list (char *username, List *newlist, List *reviewlist);
void
release_newlist (List *newlist);
void
release_reviewlist (List *reviewlist);
Card *
get_first_card (List *list);
Card *
select_card (List *newlist, List *reviewlist);
void
insert_card (Card* card, List* list);
void
save_list (FILE* fp, List *list);
void 
save_all_list (char * username, List *newlist, List *reviewlist);
int 
list_get_size (List *list);
void
shuffle_list(List *list);

#endif
