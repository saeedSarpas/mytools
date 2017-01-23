#ifndef TRIPLECASCADE_H
#define TRIPLECASCADE_H


#include "./../../../avltree/avltree.h"
#include "./../../../vector/vector.h"


vector** triplecascade(avltree*, avltree*, int,
                       avltree**, avltree**, avltree**, int);
void dispose_triplecascade(vector***);



#endif /* TRIPLECASCADE_H */
