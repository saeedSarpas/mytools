/*
 * example/matchinghalos/load_mh_test.c
 * test_for: example/matchinghalos/load_mh.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./load_mh.h"
#include "./../../io/open_file.h"
#include "./../../avltree/avl_inorder_traversal.h"


#define FNAME "./matches.ascii"


void test_results(avlnode*, void*);


Describe(avl_delete);


BeforeEach(avl_delete)
{
  FILE *file = open_file(FNAME, "w");
  fprintf(file, "Primary input: ./path/to/first/snapshot\n");
  fprintf(file, "Secondary input: ./path/to/second/snapshot\n");
  fprintf(file, "Mass offset: 5.000000\n");
  fprintf(file, "Maximum halo displacement: 5.000000\n");
  fprintf(file, "Initial volume grid: 128\n");
  fprintf(file, "num of halos in primary input: 8\n");
  fprintf(file, "num of halos in secondary input: 9\n");
  fprintf(file, "num of found matches: 7\n");
  fprintf(file, "#p_haloid p_mass s_haloid s_mass goodness\n");

  int i;
  for(i = 0; i < 7; i++)
    fprintf(file, "%7d 2.34567890000000e+10 %7d 2.34567890000000e+10 %6.2f\n",
            (int)pow(10, i), (int)pow(10, i) + 1, 100.00 / (i+1));

  fclose(file);
}


AfterEach(avl_delete)
{
  remove(FNAME);
}


Ensure(avl_delete, keeps_the_tree_bst)
{
  avltree *mh = load_mh(FNAME);

  int i = 0;

  avl_inorder_traversal(mh, test_results, &i);
}


void test_results(avlnode *node, void *data)
{
  int i = *(int*)data;
  assert_that(*(int*)node->key, is_equal_to(pow(10, i) + 1));

  match *match_p = (match*)node->data;
  assert_that(match_p->matchid, is_equal_to(pow(10, i)));
  assert_that(match_p->goodness, is_equal_to(100.0 / (i+1)));

  (*(int*)data)++;
}
