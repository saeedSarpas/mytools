#ifndef VECTOR_H
#define VECTOR_H


#include <stddef.h>


typedef struct _vector{
	void *elems;
	size_t elem_size;
	size_t log_length;
	size_t alloc_length;
} vector;


struct _vector* vector_new(size_t, size_t);
void vector_dispose(struct _vector*);
void vector_push(struct _vector*, void*);
void* vector_pop(struct _vector*);
void* vector_get_elem(struct _vector*, int);


#endif /* VECTOR_H */
