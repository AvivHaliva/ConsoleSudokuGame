
#include<stdio.h>
#include<stdlib.h>
#include "LinkedList.h"

/***** LinkedList inner functions *****/

/*
 * create a sentinel InnerListNode and return a pointer to it.
 */
InnerListNode* createSentinel() {
	return createInnerListNode(-1,-1,-1,-1);
}

/*
 * frees all of the inner linked list memory
 */
void freeInnerLinkedList(InnerListNode * node){
	InnerListNode * temp;

	while (node != NULL){
		temp = node;
		node = node->next;
		free(temp);
	}
}

/***** LinkedList.h implementation *****/

InnerListNode* createInnerListNode(int i, int j, int z, int oldZ){
	InnerListNode * node = (InnerListNode*) malloc(sizeof(InnerListNode));
    if (node == NULL){
		printMemoryAllocationError();
		exit(0);
	}
	node->cellChange[0] = i;
	node->cellChange[1] = j;
	node->cellChange[2] = z;
	node->cellChange[3] = oldZ;
	node->next = NULL;
	return node;
}

ListNode* createNewListNode(InnerListNode* x) {
	ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL){
		printMemoryAllocationError();
		exit(0);
	}
	newNode->move = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

void insertAtTail(LinkedList * list, InnerListNode * x) {
	ListNode* tailPrev;
	ListNode* newNode = createNewListNode(x);

	/* if the list is empty add new node as first node */
	if (list->head == NULL){
		/* create list head sentinel */
		list->head = createNewListNode(createSentinel());
		/* create list tail sentinel */
		list->tail = createNewListNode(createSentinel());
		/* sets the new node between the list heas sentinel and the list tail sentinel */
		list->head->next = newNode;
		newNode->next = list->tail;
		list->tail->prev = newNode;
		newNode->prev = list->head;
	}
	else{
	    /* list has at least one node, add the node in the end
	     * set newNode as the list new "before last" node -
	     * between the previous "before last node" and the list tail sentinel
	     */
		tailPrev = list->tail->prev;
		tailPrev->next = newNode;
		newNode->prev = tailPrev;
		newNode->next = list->tail;
		list->tail->prev = newNode;
	}
	/* update list movePointer to the newNode */
	list->movePointer = newNode;
}

void setMovePointerBackward(LinkedList * list) {
	list->movePointer = list->movePointer->prev;
}

void setMovePointerForward(LinkedList * list) {
	list->movePointer = list->movePointer->next;
}


void freeLinkedListFromStartPoint(ListNode * start){
	ListNode * temp;
	ListNode * curr = start;
	ListNode * startPrev;

	if(start == NULL) {
		return;
	}

	startPrev = start->prev;
	/* while curr != list tail sentinel */
	while (curr->next != NULL){
		temp =  curr;
		curr = curr->next;
		freeInnerLinkedList(temp->move);
		free(temp);
	}
	/* update pointers:
	 * 1. startPrev->next = list tail sentinel
	 * 2. list tail sentinel->prev = startPrev
	 */
	if(startPrev != NULL) {
		startPrev->next = curr;
		curr->prev = startPrev;
	}
}

void freeLinkedList(LinkedList * list) {
	if(list != NULL) {
		freeLinkedListFromStartPoint(list->head);
		if(list->tail != NULL) {
			freeInnerLinkedList(list->tail->move);
			free(list->tail);
		}
		free(list);
	}
}

void deleteAfterPointer(LinkedList * list) {
	if(list->movePointer != NULL) {
		freeLinkedListFromStartPoint(list->movePointer->next);
	}
}

int checkUndoAvailable(LinkedList * list) {
	if(list->movePointer == NULL ||
			list->movePointer->prev == NULL) {
		return 0;
	}
	return 1;
}

int checkRedoAvailable(LinkedList * list) {
	if(list->movePointer == NULL ||
		list->movePointer->next == NULL ||
		list->movePointer->next->next == NULL) {
		return 0;
	}
	return 1;
}

void addSetMoveToMovesList(LinkedList * list, int i, int j, int z, int oldZ){
	InnerListNode * node;
	/* clear any move (ListNode) beyond list->movePointer from the list */
	deleteAfterPointer(list);
	/* create innerListNode describing the current move */
	node = createInnerListNode(i,j,z,oldZ);
	/* insert the new move ListNode to the end of the list */
	insertAtTail(list, node);
}


/***** Stack implementation *****/

LinkedList * createStack() {
	LinkedList * list = (LinkedList*) malloc(sizeof(LinkedList));
    if (list == NULL){
		printMemoryAllocationError();
		exit(0);
	}
	list->head = NULL;
	list->tail = NULL;
	list->movePointer = NULL;
	return list;
}

void pushStack(LinkedList * stack, int i, int j, int val) {
	ListNode* newNode = createNewListNode(createInnerListNode(i, j, val, 0));

	if(stack->head == NULL) {
		stack->head = newNode;
	}
	else {
		newNode->next = stack->head;
		stack->head->prev = newNode;
		stack->head = newNode;
	}
}

int isEmptyStack(LinkedList * stack) {
	if(stack == NULL || stack->head == NULL) {
		return 1;
	}
	return 0;
}

int topStack(LinkedList * stack, int * i, int * j, int * val) {
	if(isEmptyStack(stack)) {
		return 0;
	}
    *i = stack->head->move->cellChange[0];
    *j = stack->head->move->cellChange[1];
    *val = stack->head->move->cellChange[2];
	return 1;
}

void popStack(LinkedList * stack) {
	ListNode * node;

	if(isEmptyStack(stack)) {
		return;
	}

	node = stack->head;
	if(node->next == NULL) {
		stack->head = NULL;
	}
	else {
		stack->head = node->next;
		stack->head->prev = NULL;
	}

	node->next = NULL;
	node->prev = NULL;
	freeInnerLinkedList(node->move);
	free(node);
}

void freeStack(LinkedList * stack) {

	if(stack == NULL) {
		return;
	}

	while (isEmptyStack(stack) == 0){
		popStack(stack);
	}
	free(stack);
}
