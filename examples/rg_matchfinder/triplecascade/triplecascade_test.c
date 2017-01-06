/*
 * examples/rg_matchfinder/triplecascade/triplecascade_test.c
 * test_for: examples/rg_matchfinder/triplecascade/triplecascade.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./triplecascade.h"
#include "./../../../avltree/avltree.h"
#include "./../../../avltree/avl_insert.h"
#include "./../../../memory/allocate.h"


#define NSNAPS 11


avltree **matches_256 = NULL;
avltree **matches_512 = NULL;
avltree **matches_1024 = NULL;
avltree *matches_256_512 = NULL;
avltree *matches_512_1024 = NULL;


Describe(triplecascade);


BeforeEach(triplecascade)
{
  matches_256 = allocate(NSNAPS, sizeof(*matches_256));
  matches_512 = allocate(NSNAPS, sizeof(*matches_512));
  matches_1024 = allocate(NSNAPS, sizeof(*matches_1024));

  int i, j;
  for(i = 0; i < NSNAPS; i++){
    j = i + 1;

    matches_256[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_256[i], &j, &i, 1, sizeof(int));

    matches_512[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_512[i], &j, &i, 1, sizeof(int));

    matches_1024[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_1024[i], &j, &i, 1, sizeof(int));
  }

  int haloid = NSNAPS;

  matches_256_512 = new_avltree(set_int_key, compare_int_keys);
  avl_insert(matches_256_512, &haloid, &haloid, 1, sizeof(int));

  matches_512_1024 = new_avltree(set_int_key, compare_int_keys);
  avl_insert(matches_512_1024, &haloid, &haloid, 1, sizeof(int));
}


AfterEach(triplecascade)
{
  int i;
  for(i = 0; i < NSNAPS; i++){
    dispose_avltree(&matches_256[i]);
    dispose_avltree(&matches_512[i]);
    dispose_avltree(&matches_1024[i]);
  }

  free(matches_256);
  free(matches_512);
  free(matches_1024);

  dispose_avltree(&matches_256_512);
  dispose_avltree(&matches_512_1024);
}


Ensure(triplecascade, returns_the_right_triplecascade_for_a_trivial_input)
{
  vector **cascades = triplecascade(matches_256_512, matches_512_1024,
      matches_256, matches_512, matches_1024, NSNAPS);

  int i;
  for(i = 0; i < NSNAPS; i++)
    assert_that(cascades[i]->logLength, is_equal_to(1));

  assert_that(cascades[NSNAPS]->logLength, is_equal_to(3));

  dispose_triplecascade(&cascades);
}
