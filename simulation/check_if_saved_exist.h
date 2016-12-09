#ifndef CHECK_IF_SAVED_EXIST_H
#define CHECK_IF_SAVED_EXIST_H


#include <stdbool.h>


#define SAVED_SUFFIX "_saved"


bool check_if_saved_exist(char*);
char* saved_snap_path(char*);


#endif /* CHECK_IF_SAVED_EXIST_H */
