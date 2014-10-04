/*
 * sorted-list.c
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return (i1 < i2) ? -1 : ((i1 > i2) ? 1 : 0);
}

int compareDoubles(void *p1, void *p2)
{
	double d1 = *(double*)p1;
	double d2 = *(double*)p2;

	return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

//Destructor functions
void destroyBasicTypeAlloc(void *p){
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for allocated memory (malloc,calloc,etc.)
	free(p);
}

void destroyBasicTypeNoAlloc(void *p) {
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for memory that has not been allocated (e.g., "int x = 5;SLInsert(mylist,&x);SLRemove(mylist,&x);")
	return;
}

void printSLInts(SortedListPtr sl) {
	SortedListIteratorPtr iter = SLCreateIterator(sl);
	int *i = SLGetItem(iter);
	if (i == NULL) {
		printf("NULL\n");
		SLDestroyIterator(iter);
		return;
	}
	printf("%d -> ", *i);
	while ( (i = SLNextItem(iter)) != NULL) {
		printf("%d -> ", *i);
	}
	printf("NULL\n");
	SLDestroyIterator(iter);
	return;
}

int test1() {
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeAlloc);
	// adding from smallest to largest
	for (int i = 0; i < 5; i++) {
		int *x = malloc(sizeof(int));
		*x = i;
		SLInsert(sl, x);
		printSLInts(sl);
	}
	int *a = malloc(sizeof(int)); *a = 10;
	int *b = malloc(sizeof(int)); *b = 6;
	int *c = malloc(sizeof(int)); *c = -2;
	
	// add to head
	SLInsert(sl, a);
	printSLInts(sl);

	// add in middle
	SLInsert(sl, b);
	printSLInts(sl);

	// adding twice
	SLInsert(sl, c);
	printSLInts(sl);
	SLInsert(sl, c);
	printSLInts(sl);

	// double remove
	SLRemove(sl, c); // c is freed at this point
	printSLInts(sl);
	int n = -2;
	SLRemove(sl, &n);
	printSLInts(sl);
	
	// trying to remove something that was never there in the first place
	// in the above example, c will be freed if there were no more references to the node when it was removed
	//int *x = malloc(sizeof(int)); *x = 17;
	int x = 17;
	SLRemove(sl, &x);
	printSLInts(sl);

	// remove head
	SLRemove(sl, a);
	printSLInts(sl);
	SLDestroy(sl);

	printf("-------------------\n\n");
	return 0;
}

int test2() {
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeNoAlloc);
	int a = 10;
	// removing when list is empty
	SLRemove(sl, &a);
	printSLInts(sl);
	SLInsert(sl, &a);
	printSLInts(sl);
	SLRemove(sl, &a);
	printSLInts(sl);

	SLDestroy(sl);

	printf("-------------------\n\n");
	return 0;
}

int test3() {
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeAlloc);
	// adding from largest to smallest
	for (int i = 10; i >-3; i--) {
		int *x = malloc(sizeof(int));
		*x = i;
		SLInsert(sl, x);
		printSLInts(sl);
	}

	// remove from next to last and then last
	int *z = malloc(sizeof(int)); *z = -1;
	SLRemove(sl, z);
	printSLInts(sl);
	int *y = malloc(sizeof(int)); *y = -2;
	SLRemove(sl, y);
	printSLInts(sl);

	free(z); free(y);
	
	SLDestroy(sl);

	printf("-------------------\n\n");
	return 0;
}
int test4() {
	// place iterators and continue iterating
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeAlloc);
	for (int i = 10; i >-3; i--) {
		int *x = malloc(sizeof(int));
		*x = i;
		SLInsert(sl, x);
		printSLInts(sl);
	}

	// place iter1
	SortedListIteratorPtr iter1 = SLCreateIterator(sl);
	SLNextItem(iter1);
	SLNextItem(iter1);
	printf("iter1 %d\n", *(int*)SLGetItem(iter1));

	// place iter2 in front of iter1
	SortedListIteratorPtr iter2 = SLCreateIterator(sl);
	SLNextItem(iter2);
	SLNextItem(iter2);
	SLNextItem(iter2);
	printf("iter2 %d\n", *(int*)SLGetItem(iter2));

	// remove at iter1
	SLRemove(sl, SLGetItem(iter1));
	printSLInts(sl);
	// remove at iter2
	SLRemove(sl, SLGetItem(iter2));
	printSLInts(sl);

	// advance iter1 and it's the one that was just removed
	if (SLNextItem(iter1) == SLGetItem(iter2)) {
		printf("At hidden, removed but not destroyed node with data: %d == %d\n", *(int*)SLGetItem(iter1), *(int*)SLGetItem(iter2));
	}
	printSLInts(sl);

	// advance iter1 and iter 2 and they're still equal
	printf("iter1, iter2 still equal after advancement: %d == %d\n", *(int*)SLNextItem(iter1), *(int*)SLNextItem(iter2));

	SLDestroyIterator(iter1);
	SLDestroyIterator(iter2);

	SLDestroy(sl);
	printf("-------------------\n\n");
	return 0;
}
int test5() {
	// create and destroy
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeAlloc);
	
	for (int i = 50; i>0; i--) {
		int *x = malloc(sizeof(int)); *x = i;
		SLInsert(sl, x);
	}
	SortedListIteratorPtr iter = SLCreateIterator(sl);
	printSLInts(sl);
	// remove everything from the list, but the iterator is keeping the 50 there
	for (int i = 50; i>0; i--) {
		int *x = malloc(sizeof(int)); *x = i;
		SLRemove(sl, x);
		free(x);
	}
	printSLInts(sl);
	//Iterator is still at 50
	printf("Iterator is keeping removed nodes alive. current: %d, next: %d\n", *(int*)SLGetItem(iter), *(int*)SLNextItem(iter));
	// 50 is destroyed once the SLNextItem runs
	// everything gets destroyed after this
	SLDestroyIterator(iter);


	SLDestroy(sl);

	printf("-------------------\n\n");
	return 0;
}
int test() {
	// create and destroy
	SortedListPtr sl = SLCreate(compareInts, destroyBasicTypeNoAlloc);
	printSLInts(sl);
	SLDestroy(sl);

	printf("-------------------\n\n");
	return 0;
}
int main() {
	test();
	test1();
	test2();
	test3();
	test4();
	test5();
	return 0;
}
