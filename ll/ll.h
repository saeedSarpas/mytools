#ifndef LL_H
#define LL_H


// Type of linked list
#define LL_INT sizeof(int)
#define LL_FLOAT sizeof(float)
#define LL_DOUBLE sizeof(double)


typedef enum {
  LL_FRONT,
  LL_BACK
} llmode;


typedef struct _llnode
{
  struct _llnode *next;
  union {
    char *zero;
    int ikey;
    float fkey;
    double dkey;
  };
  void *data;
} llnode;


typedef struct _ll
{
  llnode *head;
  int sizeofkey;
  int len;
} ll;


ll* new_ll(int);
void dispose_ll(ll**);


#endif /* LL_H */
