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
#include "./../../../vector/vector_get.h"
#include "./../../../memory/allocate.h"
#include "./../../../ll/ll.h"


#define NSNAPS 11
#define NHALOS (NSNAPS + 1)


avltree **matches_256 = NULL;
avltree **matches_512 = NULL;
avltree **matches_1024 = NULL;
avltree *matches_512_256 = NULL;
avltree *matches_1024_512 = NULL;

int halo256_id, halo512_id, halo1024_id;


Describe(triplecascade);


BeforeEach(triplecascade)
{
  matches_256 = allocate(NSNAPS, sizeof(*matches_256));
  matches_512 = allocate(NSNAPS, sizeof(*matches_512));
  matches_1024 = allocate(NSNAPS, sizeof(*matches_1024));

  int i, down_id, up_id;
  for(i = 0; i < NSNAPS; i++){
    down_id = i;
    up_id = i + 1;

    matches_256[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_256[i], &up_id, &down_id, 1, sizeof(int));

    down_id++;
    up_id++;

    matches_512[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_512[i], &up_id, &down_id, 1, sizeof(int));

    down_id++;
    up_id++;

    matches_1024[i] = new_avltree(set_int_key, compare_int_keys);
    avl_insert(matches_1024[i], &up_id, &down_id, 1, sizeof(int));
  }

  halo1024_id = up_id;
  halo512_id = halo1024_id - 1;
  halo256_id = halo512_id - 1;

  matches_512_256 = new_avltree(set_int_key, compare_int_keys);
  avl_insert(matches_512_256, &halo256_id, &halo512_id, 1, sizeof(int));

  matches_1024_512 = new_avltree(set_int_key, compare_int_keys);
  avl_insert(matches_1024_512, &halo512_id, &halo1024_id, 1, sizeof(int));
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

  dispose_avltree(&matches_512_256);
  dispose_avltree(&matches_1024_512);
}


Ensure(triplecascade, returns_the_right_triplecascade_for_a_trivial_input)
{
  vector **cascades = triplecascade(matches_512_256,
                                    matches_1024_512,
                                    halo256_id + 1,
                                    matches_256,
                                    matches_512,
                                    matches_1024,
                                    NSNAPS);

  int i;
  for(i = 0; i < NSNAPS; i++)
    assert_that(cascades[i]->logLength, is_equal_to(1));

  assert_that(cascades[NHALOS - 1]->logLength, is_equal_to(3));

  ll *list = NULL;

  list = vector_get(cascades[NSNAPS], 0);
  assert_that(list->len, is_equal_to(NSNAPS + 1));
  assert_that(list->head->ikey, is_equal_to(halo256_id));

  list = vector_get(cascades[NSNAPS], 1);
  assert_that(list->len, is_equal_to(NSNAPS + 1));
  assert_that(list->head->ikey, is_equal_to(halo512_id));

  list = vector_get(cascades[NSNAPS], 2);
  assert_that(list->len, is_equal_to(NSNAPS + 1));
  assert_that(list->head->ikey, is_equal_to(halo1024_id));

  dispose_triplecascade(&cascades);
}
