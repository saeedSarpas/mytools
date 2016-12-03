#ifndef VECTOR_H
#define VECTOR_H


typedef struct _vector{
	void *elems;
	unsigned int elemSize;
	unsigned int logLength;
	unsigned int allocLength;
} vector;


vector* new_vector(unsigned int, unsigned int);
void dispose_vector(vector**);


#endif /* VECTOR_H */
