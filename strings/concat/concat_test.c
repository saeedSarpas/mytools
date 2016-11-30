/*
 * strings/concat_test.c
 * test_for: strings/concat.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <string.h>


#include "./concat.h"


#define STRING0 "string0"
#define STRING1 "string1"
#define STRING2 "string2"
#define STRING3 "string3"
#define STRING4 "string4"
#define STRING5 "string5"
#define LENGTH 6


static char *strings[LENGTH];
static char *result;


Describe (concat);


BeforeEach(concat)
{
  strings[0] = strdup(STRING0);
  strings[1] = strdup(STRING1);
  strings[2] = strdup(STRING2);
  strings[3] = strdup(STRING3);
  strings[4] = strdup(STRING4);
  strings[5] = strdup(STRING5);

  result = malloc(strlen(STRING0) * LENGTH + 1);

  int i;
  for (i = 0; i < LENGTH; i++)
    strcat(result, strings[i]);
}


AfterEach(concat)
{
  int i;
  for (i = 0; i < LENGTH; i++)
    free(strings[i]);

  free(result);
}


Ensure(concat, concats_strings_correctly)
{
  char *base = concat(LENGTH,
                      strings[0], strings[1], strings[2], strings[3], strings[4], strings[5]
  );

  assert_true(strcmp(base, result) == 0);

  free(base);
}
