
#ifndef LIST__H
#define LIST__H


#ifndef NULL
#define NULL 0
#endif

typedef void* dtype;

struct node_t {
    dtype data;
    struct node_t *next;
};

typedef struct node_t node;

typedef struct {
    node *head;
    node *currentp; //this is for "seek" like ops
} list;

list*
list_create(void);
/* create a list
*/

int
list_insert(list  *l,
	    dtype data);

dtype
list_getnext(list *l);

void
list_reset(list *l);

#endif
