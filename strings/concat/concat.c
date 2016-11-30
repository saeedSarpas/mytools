/*
 * strings/concat.c
 * test_file: strings/concat_test.c
 *
 * Concatenating strings
 *
 * param: count number of strings to be concatenated (it should be greater than
 * 2)
 * param: ... a series of strings
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "./../../memory/allocate.h"


char* concat(int count, ...)
{
  va_list list;
  char **strings = malloc(count * sizeof(char *));

  va_start(list, count);

  int i, length = 0;
  for(i = 0; i < count; i++){
    strings[i] = va_arg(list, char *);
    length += strlen(strings[i]);
  }

  char *result = allocate(length + 1, sizeof(char));

  for(i = 0; i < count; i++)
    strcat(result, strings[i]);

  free(strings);
  va_end(list);

  return result;
}
