#ifndef NODE_H
#define NODE_H

/*
 * When your sorted list is used to store objects of some type, since the
 * type is opaque to you, you will need a comparator function to order
 * the objects in your sorted list.
 *
 * You can expect a comparator function to return -1 if the 1st object is
 * smaller, 0 if the two objects are equal, and 1 if the 2nd object is
 * smaller.
 *
 * Note that you are not expected to implement any comparator or destruct
 * functions.  You will be given a comparator function and a destruct
 * function when a new sorted list is created.
 */

typedef int (*CompareFuncT)( void *, void * );
typedef void (*DestructFuncT)( void * );

/* Node type */
struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
    unsigned int refcount;
	DestructFuncT destroy_data;
};
typedef struct Node* NodePtr;

NodePtr NodeCreate(void *data, DestructFuncT destroy_data);
void NodeDestroy(NodePtr node);

void NodeLink(NodePtr n1, NodePtr n2);
void NodeUnlink(NodePtr n1, NodePtr n2);

void NodeSetHead(NodePtr node);
void NodeUnsetHead(NodePtr node);

#endif
