// Added an enqueueToFront function.

// Queue Code
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

int size( Queue *Q ) {
	return Q->sz;
}

int isEmpty( Queue *Q ){
	if( Q->sz == 0 ) return 1;
	return 0;
}

void enqueue( Queue *Q, Object elem ){
	Node *v = (Node*)malloc(sizeof(Node));/*Allocate memory for the Node*/
	if( !v ){
		printf("ERROR: Insufficient memory\n");
		return;
	}
	v->element = elem;
	v->next = NULL;
	if( isEmpty(Q) ) Q->head = v;
	else Q->tail->next = v;
	Q->tail = v;
	Q->sz++;
}

void enqueueToFront( Queue *Q, Object elem ){
	Node *v = (Node*)malloc(sizeof(Node));/*Allocate memory for the Node*/
	if( !v ){
		printf("ERROR: Insufficient memory\n");
		return;
	}
	if ( isEmpty(Q) ) {
		v->element = elem;
		v->next = NULL;
		Q->head = v;
	} 
	else {
		v->element = elem;
		v->next = Q->head;
		Q->head = v;
	}
	Q->sz++;
}

Object dequeue( Queue *Q ){
	Node *oldHead;
	Object temp;
	if( isEmpty(Q) ){
		printf("ERROR: Queue is empty\n");
		return temp;
	}
	oldHead = Q->head;
	temp = Q->head->element;
	Q->head = Q->head->next;
	free(oldHead);
	Q->sz--;
	return temp;
}

Object first( Queue *Q ){
	if( isEmpty(Q) ){
		Object temp;
		printf("ERROR: Queue is empty\n");
		return temp;
	}
	return Q->head->element;
}

void destroyQueue( Queue *Q ){
	while( !isEmpty(Q) ) dequeue(Q);
}

/*A different visit function must be used for each different datatype.*/
/*The name of the appropriate visit function is passed as an argument */
/*to traverseQueue.                                                   */
void visitPCB( Object elem ){
	printf("\nJob %d ",elem.jobnum);
}
/*The following function isn't part of the Queue ADT, however*/
/*it can be useful for debugging.                            */
void traverseQueue( Queue *Q, void (*visit)(Object elem) ){
	printf("\nThe queue currently consists of: ");
	Node *current = Q->head;
	while( current ){
		visit(current->element);
		current = current->next;
	}
	printf("\n");
}
