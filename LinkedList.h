/************************************
* FILES NAMES :        LinkedList.c , LinkedList.h
*
* DESCRIPTION :
*       Defining LinkedList and List Nodes structures and operations: create, insert , delete and free memory.
*       Support stack operations based on those structures and operations.
*       The files goal is to support the sudoku game redo-undo list, as well as the BT algorithm stack.
*
* PUBLIC FUNCTIONS :
*       InnerListNode* createInnerListNode(int i, int j, int z, int oldZ)
*       ListNode* createNewListNode(InnerListNode* x)
*       void insertAtTail(LinkedList * list, InnerListNode* x)
*       void setMovePointerForward(LinkedList * list)
*       void setMovePointerBackward(LinkedList * list)
*       void freeLinkedListFromStartPoint(ListNode * start)
*       void freeLinkedList(LinkedList * list)
*       void deleteAfterPointer(LinkedList * list)
*
*       int checkUndoAvailable(LinkedList * list)
*       int checkRedoAvailable(LinkedList * list)
*       void addSetMoveToMovesList(LinkedList * List, int i, int j, int z, int oldZ)
*
*       LinkedList * createStack()
*       void pushStack(LinkedList * stack, int i, int j, int val)
*       int isEmptyStack(LinkedList * stack)
*       int topStack(LinkedList * stack, int * i, int * j, int * val)
*       void popStack(LinkedList * stack)
*       void freeStack(LinkedList * stack)
*
* NOTES :
*       1. LinkedList.h contains the definition of the following:
*           a. InnerListNode struct - a node that is meant to describe cell change (cell indexes, previous value and new value).
*              each InnerListNode describes a single cell change. Also has a pointer to the next and the prev cell changes,
*              containing the values as described in the full documentation below.
*           b. ListNode struct - a node that is meant to describe all the cell changes in a single command.
*              Also has a pointer to the prev and the next command that caused cell changes. A more detailed documentation can be found below.
*           c. LinkedList struct - a list of ListNodes. Has a pointer to the head, tail and current move. Used for game redo-undo list ("moves")
*              and also for the BT stack implementation
*
************************************/

#include "Command.h"
#define INNER_LIST_NODE_ARR_SIZE 4

/*
 * InnerListNode is a struct describing all the cell changes
 * in one game command (Set, generate, autofill etc.)
 * 1. cellChange
 * 		a. an array that contains the following values:
 * 			i.    cellChange[0] - cell row index
 * 			ii.	  cellChange[1] - cell col index
 * 			iii.  cellChange[2] - cell value AFTER command execution
 * 			iiii. cellChange[3] - cell value BEFORE command execution
 * 		b. a pointer to the next InnerListNode:
 * 			i.  in set command - next is always NULL
 * 			ii. in autofill or generate command - next is not null
 * 				if more than one cell has change in the command execution
 */
typedef struct InnerListNode{
    int cellChange[INNER_LIST_NODE_ARR_SIZE];
    struct InnerListNode* next;
}InnerListNode;

/*
 * ListNode is a struct that contains:
 * 1. move: pointer to InnerListNode which describes the cell changes
 * 			that has been made in the command execution
 * 2. next, prev: pointer to the next / prev move that changed some cell values,
 * 				  in order of execution
 */
typedef struct ListNode{
    InnerListNode * move;
    struct ListNode* next;
    struct ListNode* prev;
}ListNode;

/*
 * LinkedList is a struct that contains:
 *
 * 	1. head - pointer to the list start
 * 	2. tail - pointer to the list end
 * 	3.movePointer - pointer to the current move
 */

typedef struct LinkedList{
    /*
     * pointer to head node
     * */
    ListNode* head;
    /*
     * pointer to tail node
     * */
    ListNode* tail;
    /*
     * pointer to the current move, used for undo and redo.
     * */
    ListNode* movePointer;

}LinkedList;

/*
 * get cell indexes <i,j>, cell new value (z) and cell previous value (oldZ)
 * creates a new InnerListNode, in the format described on InnerListNode struct documentation
 */
InnerListNode* createInnerListNode(int i, int j, int z, int oldZ);


/*
 * creates a new node and returns a pointer to it
 * */
ListNode* createNewListNode(InnerListNode* x) ;

/*
 * creates and inserts a new node to the tail of the list.
 *
 * 1. if the list is empty add new node as first node.
 * 	  i.   creates a new node as well as head and tail sentinels
 * 	  ii.  sets the new node between the head sentinel and the tail sentinel
 * 	  iii. sets the list movePointer to the new node
 * 	  * list desc.: NULL <- headSentinel <-> new node <-> tailSentinel -> NULL *
 *
 * 2. if the list has at least one nod, add the node in the end
 * 	  i. the new node will be setted between the list tail sentinel preceding node
 * 	  	 and the list tail sentinel
 * 	  ii. sets the list movePointer to the new node
 * 	  * list desc.: NULL <- headSentinel <-> .... <->
 * 	  				 tailSentinelPrev <-> new node <-> tailSentinel -> NULL *
 * */
void insertAtTail(LinkedList * list, InnerListNode* x);

/*
 * sets the currMovePointer one node forward
 * */
void setMovePointerForward(LinkedList * list);

/*
 * sets the currMovePointer one node backwards.
 * */
void setMovePointerBackward(LinkedList * list);

/*
 * gets a pointer to start, a ListNode , and clears any move beyond it from the list,
 * not including the list tail sentinel.
 * for each deleted ListNode, also delete and free its innerLInkedList.
 * NOTE:
 * 1. if start is NULL the function doesn't perform any operation
 * 2. if start isn't the head sentinel (start->prev is not NULL) -
 * 		start->prev is set as the previous node of the tail sentinel
 * 		start->prev->next is seat as the tail sentinel
 *
 */
void freeLinkedListFromStartPoint(ListNode * start);

/*
 * frees all of the linked list memory
 * 1. first, frees starting the list head sentinel using freeLinkedListFromStartPoint
 * 2. in the end, frees the list tail sentinel innerLinkedList
 * 	  and then the tail sentinel node itself
 * 3. NOTE: if the list is empty (list == NULL or list->head == NULL),
 * 	  the function doesn't execute resources freeing
 */
void freeLinkedList(LinkedList * list);

/*
 * gets a pointer to a list (LinkedList) and if the list movePointer is not NULL,
 * clears any move beyond the movePointer from the list
 */
void deleteAfterPointer(LinkedList * list);


/*
 * gets a pointer to a LinkedList and checks if undo command
 * is available:
 * return 1 iff there is a preceding node to currMovePointer in the list
 * i.e. there is a preceding operation in the undo-redo list that can be done
 */
int checkUndoAvailable(LinkedList * list);

/*
 * gets a pointer to a LinkedList and checks if redo command
 * is available:
 * return 1 iff there is a successive node to currMovePointer in the list
 * i.e. there is a successive operation in the undo-redo list that can be done
 */
int checkRedoAvailable(LinkedList * list);

/*
 * gets a pointer to the LinkedList, as well as cell indexes <i,j>,
 * cells new value (z) and cell previous value before the set command execution (oldZ).
 * The function clears any move beyond the current move from the list,
 * then add the new move to the end of the list and mark it as the current move.
 */
void addSetMoveToMovesList(LinkedList * List, int i, int j, int z, int oldZ);

/*
 *  allocates a new linked listed and returns a pointer to it
 */
LinkedList * createStack();

/*
 * adding new item (with values i,j,val) as the head of the stack
 */
void pushStack(LinkedList * stack, int i, int j, int val);

/*
 * return 1 iff the stack is empty
 */
int isEmptyStack(LinkedList * stack);

/*
 * get the values of the head of the stack and put them in i,j,val
 * return 1 iff the stack has a head
 */
int topStack(LinkedList * stack, int * i, int * j, int * val);

/*
 * remove the head of the stack and free its memory
 */
void popStack(LinkedList * stack);

/*
 * free all the memory of the stack
 */
void freeStack(LinkedList * stack);
