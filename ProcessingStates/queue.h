typedef struct{
	int jobnum;
	int bursts[10];
	char privateFIFO[14];
	int burstpos;	// position in the burst array
	float totalBursts;	
	float utilization;
} Object;

typedef struct node{  /*Nodes stored in the linked list*/
	Object element;
	struct node *next;
} Node;

typedef struct queue{ /*A struct facilitates passing a queue as an argument*/
	Node *head;       /*Pointer to the first node holding the queue's data*/
	Node *tail;       /*Pointer to the last node holding the queue's data*/
	int sz;           /*Number of nodes in the queue*/
} Queue;

int size( Queue *Q );

int isEmpty( Queue *Q );

void enqueue( Queue *Q, Object elem );

void enqueueToFront( Queue *Q, Object elem );

Object dequeue( Queue *Q );

Object first( Queue *Q );

void destroyQueue( Queue *Q );

/*A different visit function must be used for each different datatype.*/
/*The name of the appropriate visit function is passed as an argument */
/*to traverseQueue.                                                   */
void visitPCB( Object elem );

/*The following function isn't part of the Queue ADT, however*/
/*it can be useful for debugging.                            */
void traverseQueue( Queue *Q, void (*visit)(Object elem) );
