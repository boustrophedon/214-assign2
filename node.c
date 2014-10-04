#include <stdlib.h>
#include <stdio.h>

#include "node.h"


NodePtr NodeCreate(void *data, DestructFuncT destroy_data) {
    NodePtr node = malloc(sizeof(struct Node));
    node->data = data;
    node->prev = NULL;
    node->next = NULL;
    node->refcount = 0;
	node->destroy_data = destroy_data;

    return node;
}

void NodeDestroy(NodePtr node) {
	if (node->data != NULL) {
    	node->destroy_data(node->data);
	}
	if (node->next != NULL) {
		NodeUnlink(node, node->next);
	}
    free(node);
}

void NodeLink(NodePtr n1, NodePtr n2) {
	n1->next = n2;
	if (n2 != NULL) {
		(n2->refcount)++;
	}
	return;
}
void NodeUnlink(NodePtr n1, NodePtr n2) {
	n1->next = NULL;
	if (n2 != NULL) {
		(n2->refcount)--;
		if (n2->refcount == 0) {
			NodeDestroy(n2);
		}
	}
	return;
}

void NodeSetHead(NodePtr node) {
	if (node == NULL) {
		return;
	}
	else {
		(node->refcount)++;
		return;
	}
}
void NodeUnsetHead(NodePtr node) {
	if (node == NULL) {
		return;
	}
	else {
		(node->refcount)--;
		if (node->refcount == 0) {
			NodeDestroy(node);
		}
		return;
	}
	return;
}
