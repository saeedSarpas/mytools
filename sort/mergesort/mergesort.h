#ifndef MERGESORT_H
#define MERGESORT_H


#define MS_NOT_FOUND -2
#define MS_LESS_THAN -1
#define MS_GREATER_THAN +1
#define MS_EQUALS 0


void mergesort(void*, int, int, void (*set)(void*, int, void*, int),
               int (*compare)(void*, int, void*, int), unsigned int);


#endif /* MERGESORT_H */
