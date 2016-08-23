#ifndef LOAD_ROCKSTAR_H
#define LOAD_ROCKSTAR_H


#include "./../memory/allocate.h"
#include "./../io/open_file.h"
#include "./../io/read_from.h"
#include "./rockstar_data_type.h"


struct rockstar* load_rockstar_bin(FILE*);


#endif /* LOAD_ROCKSTAR_H */
