#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "sorted-list.h"
/*  the pdf said to put the big O in the comments at the top of the source and in the readme.
 * Analyses below are assuming malloc are O(1), and $n$ is the number of nodes in the list
 * SLCreate: We just malloc and assign some variables, so it's $O(1)$
 * SLDestroy: We dealloc each item in the list, and NodeDestroy is $O(1)$, so it's $O(n)$
 * SLInsert: In the worse case we have to insert a value smaller than all the other values in the list. If our comparison function is $O(f(n))$ then the entire function is $O(nf(n))$
 * SLRemove: Again in the worst case we may have to remove a value smaller than all the other values, so as in SLInsert it's $O(nf(n))$
 * SLCreateIterator: We do a few mallocs and move some pointers, all of which are $O(1)$, so it's $O(1)$.
 * SLDestroyIterator: This may actually be $O(n)$ because of the NodeDestroy; see NodeDestroy analysis.
 * SLGetItem: We just dereference two pointers. $O(1)$
 * SLNextItem: Again we dereference some pointers and move one. $O(1)$
 * NodeCreate: We are doing some allocations. $O(1)$
 * NodeLink: We move a pointer and do an increment. $O(1)$
 * NodeUnlink and NodeDestroy: This is where it gets slightly complicated. When we unlink a node, we decrement the former node->next's refcount. If it reaches zero, we destroy that node. In NodeDestroy, if that node has a next, we do an Unlink on node->next, which may cause a destroy on that node
 * . Therefore in the worst case if we remove from the list in reverse order, starting at the smallest until we hit the head, the last NodeUnlink will
 *  cause all the reference counts to go to 0 in succession. Therefore, the worst-case runtime is $O(n)$ for both NodeUnlink and NodeDestroy. This als
 * o technically applies to NodeUnsetHead, though it never gets called without calling NodeSetHead on something else first.
 * NodeSetHead: This is just an increment so it's $O(1)$.
 */


/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list, and a destruct function that gets rid of the objects
 * once they are no longer in the list or referred to in an iterator.
 * 
 * If the function succeeds, it returns a (non-NULL) SortedListT object,
 * otherwise, it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df) {
	SortedListPtr sl = malloc(sizeof(struct SortedList));
	sl->head = NULL;
	sl->cmp_data = cf;
	sl->destroy_data = df;

	return sl;	
}

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 *
 * You need to fill in this function as part of your implementation.
 */
void SLDestroy(SortedListPtr list) {
	NodePtr cur = list->head;
	while (cur != NULL) {
		NodePtr tmp = cur->next;
		NodeSetHead(tmp);

		NodeDestroy(cur);
		cur = tmp;
	}
	free(list);
	return;
}


/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order of all objects in the list.  If the new object is equal to a subset
 * of existing objects in the list, then the subset can be kept in any
 * order.
 *
 * If the function succeeds, it returns 1, othrewise it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int SLInsert(SortedListPtr list, void *newObj) {
	NodePtr new_node = NodeCreate(newObj, list->destroy_data);
	if (list->head == NULL) {
		NodeSetHead(new_node);
		list->head = new_node;
		return 1;
	}
	else {
		NodePtr cur = list->head;
		int result = list->cmp_data(cur->data, new_node->data);
		if (result == -1) { // head is smaller than new node
			NodeSetHead(new_node);
			list->head = new_node;
			NodeLink(new_node, cur);
			NodeUnsetHead(cur);
			return 1;
		}
		else { // do the insertion part
			while (cur->next != NULL) {
				result = list->cmp_data((cur->next)->data, new_node->data);
				if (result == -1) { // next is less than new, so we insert inbetween
					NodeLink(new_node, cur->next);
					NodeUnlink(cur, cur->next);
					NodeLink(cur, new_node);
					return 1;
				}
				else if (result == 0) {
					free(new_node); // don't use NodeDestroy because that also frees the data
					return 0;
				}
				cur = cur->next;
			}
			// if we get through the while loop, we add to the end
			NodeLink(cur, new_node);
			return 1;
		}
		return 0; // shouldn't get here
	}		
}


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.  SLRemove may not change the object whose
 * pointer is passed as the second argument.  This allows you to pass
 * a pointer to a temp object equal to the object in the sorted list you
 * want to remove.
 *
 * If the function succeeds, it returns 1, otherwise it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int SLRemove(SortedListPtr list, void *newObj) {
	if (list->head == NULL) {
		return 0;
	}
	NodePtr cur = list->head;
	// if we're removing the head
	if (list->cmp_data(cur->data, newObj) == 0) {
		NodePtr tmp = cur->next;
		NodeSetHead(tmp);
		list->head = tmp;
		NodeUnsetHead(cur);

		return 1;
	}
	// else search for object
	else {
		while (cur->next != NULL) {
			int result = list->cmp_data((cur->next)->data, newObj);
			if (result == 0) { // found it, removing cur->next
				NodePtr tmp = NodeCreate(NULL, NULL);
				NodeLink(tmp, (cur->next)->next);

				NodeUnlink(cur, cur->next);
				NodeLink(cur, tmp->next);

				NodeDestroy(tmp);
				return 1;
			}
			else if (result == -1) { // newObj is bigger than next and list is sorted so it's not in the list
				return 0;
			}
			cur = cur->next;
		}
	}
	return 0;
}

/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL pointer to a
 * SortedListIterT object, otherwise it returns NULL.  The SortedListT
 * object should point to the first item in the sorted list, if the sorted
 * list is not empty.  If the sorted list object is empty, then the iterator
 * should have a null pointer.
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListIteratorPtr SLCreateIterator(SortedListPtr list) {
	if (list == NULL) {
		return NULL;
	}
	SortedListIteratorPtr iter = malloc(sizeof(struct SortedListIterator));
	iter->current = NodeCreate(NULL, NULL);
	NodeLink(iter->current, list->head);
	return iter;
}


/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affect the original list used to create
 * the iterator in any way.
 *
 * You need to fill in this function as part of your implementation.
 */

void SLDestroyIterator(SortedListIteratorPtr iter) {
	NodeDestroy(iter->current);
	free(iter);
	return;
}


/*
 * SLGetItem returns the pointer to the data associated with the
 * SortedListIteratorPtr.  It should return 0 if the iterator
 * advances past the end of the sorted list.
 * 
 * You need to fill in this function as part of your implementation.
*/

void * SLGetItem( SortedListIteratorPtr iter ) {
	if ((iter->current)->next == NULL) {
		return NULL;
	}
	else {
		return ((iter->current)->next)->data;
	}
}

/*
 * SLNextItem returns the next object in the list encapsulated by the
 * given iterator.  It should return a NULL when the end of the list
 * has been reached.
 *
 * One complication you MUST consider/address is what happens if a
 * sorted list encapsulated within an iterator is modified while that
 * iterator is active.  For example, what if an iterator is "pointing"
 * to some object in the list as the next one to be returned but that
 * object is removed from the list using SLRemove() before SLNextItem()
 * is called.
 *
 * You need to fill in this function as part of your implementation.
 */

void * SLNextItem(SortedListIteratorPtr iter) {
	// the choice I made was to only consider the iterator to be pointing at a single node
	// that is, if we have a node that gets removed, the nodes surrounding it have their pointers updated
	// but the node that was previously removed is still pointing to its former next
	// therefore the reference counts don't fall below zero, and they can't be destroyed prematurely
	void * data;
	if ((iter->current)->next == NULL) {
		data = NULL;
	}
	else {
		NodePtr tmp = NodeCreate(NULL, NULL);
		NodeLink(tmp, (iter->current->next)->next);
		NodeUnlink(iter->current, (iter->current)->next);
		NodeLink(iter->current, tmp->next);
		NodeDestroy(tmp);

		if ((iter->current)->next == NULL) { // new current is end of list
			data = NULL;
		}
		else {
			data = ((iter->current)->next)->data;
		}
	}
	return data;
}
