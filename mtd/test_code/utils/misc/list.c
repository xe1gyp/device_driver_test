
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"


list*
list_create(void)
{
    list *l;
    l = (list *) malloc(sizeof(list));
    if (l == NULL) return NULL;
    bzero(l, sizeof(list));
    l->head = NULL;
    l->currentp = l->head;
    return l;
}

int
list_insert(list *l, dtype d)
{
    node *p = l->head;
    node *n = (node *) malloc(sizeof(node));
    if (n == NULL) return -1;
    bzero(n, sizeof(node));
    n->data = d;
    n->next = NULL;
    if (l->head == NULL) {
	l->head = n;
	return 1;
    } else {
	while (p->next != NULL) {
	    p = p->next;
	}
	p->next = n;
    }
    list_reset(l);
    return 1;
}


dtype
list_getnext(list *l)
{
    node *temp;
    assert(l);
    temp = l->currentp;
    if (temp != NULL) {
	l->currentp = l->currentp->next;
	return temp->data;
    } else return NULL;
}

void
list_reset(list *l)
{
    assert(l != NULL);
    l->currentp = l->head;
    return;
}
