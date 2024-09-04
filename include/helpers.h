
typedef struct{
	int *array;
	size_t used;
	size_t size;
} Array;

void initArray(Array *a, size_t initial_size){
	a->array = malloc(initial_size * sizeof(int));
	a->used = 0;
	a->size = initial_size;
}

void insertArray(Array *a, int element){
	if (a->used == a->size){
		a->size *= 2;
		a->array = realloc(a->array, a->size * sizeof(int));
	}
	a->array[a->used++] = element;
}


typedef struct {
	int array[5];
	int start;
	int end;
	size_t used;
	size_t length;
} Queue;

void queue_init(Queue *q){
	q->start = 0;
	q->end = 0;
	q->used = 0;
	q->length = 5;
}

void queue_push(Queue *q, int item){
	if(q->used == q->length) return;
	q->array[q->end] = item;
	q->end = (q->end+1+q->length)%(q->length);
	q->used++;

}

int queue_pop(Queue *q){
	q->used--;
	int item = q->array[q->start];
	q->start = (q->start + 1)%q->length;
	return item;
}
