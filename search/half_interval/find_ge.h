#ifndef FIND_GE_H
#define FIND_GE_H


#define NOT_FOUND -1
#define LESS_THAN_T_VALUE -1
#define GREATER_THAN_T_VALUE +1
#define EQUAL_TO_T_VALUE 0


int find_ge(void*, void*, int, int (*compare)(const void*, int, const void*));


#endif /* FIND_GE_H */
