/*
 * avltree/avl_tree_test.c
 * test_for: avltree/avl_tree.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "avl_tree.h"


Describe(avl_tree);
BeforeEach(avl_tree) {}
AfterEach(avl_tree) {}


#define NUM_OF_NODES 200
#define INT_DATA 1234


static int counter;
static int old_key;


static struct avl_node* tree_constructor();
static void check_childs(struct avl_node*);
static void check_int_data(struct avl_node*);
static void increment();
static void check_ascending_order(struct avl_node*);


Ensure(avl_tree, find_works_properly)
{
  struct avl_node *root = tree_constructor();
  struct avl_node *found_node;

  int64_t i;
  for(i = 0; i < NUM_OF_NODES; i += 5){
    found_node = avl_find(root, i);
    if(found_node == NULL)
      printf("mosavie null\n");
    else
      assert_that(found_node->key, is_equal_to(i));
  }

  found_node = avl_find(root, NUM_OF_NODES);
  assert_that(found_node, is_null);

  avl_dispose(root);
}


Ensure(avl_tree, insert_keeps_the_tree_bst)
{
  struct avl_node *root = tree_constructor();
  inorder_traversal(root, check_childs);

  avl_dispose(root);
}


Ensure(avl_tree, insert_prevents_from_adding_nodes_with_same_keys)
{
  struct avl_node *root = NULL;

  int i;
  for(i = 0; i < 10; i++)
    root = avl_insert(root, 1, NULL);

  counter = 0;
  inorder_traversal(root, increment);

  assert_that(counter, is_equal_to(1));

  avl_dispose(root);
}


Ensure(avl_tree, insert_set_the_data_properly)
{
  struct avl_node *root = NULL;

  int data = INT_DATA;
  int i;
  for(i = 0; i < 10; i++)
    root = avl_insert(root, i, &data);

  inorder_traversal(root, check_int_data);

  avl_dispose(root);
}


Ensure(avl_tree, inorder_traversal_goes_through_all_nodes)
{
  struct avl_node *root = tree_constructor();

  counter = 0;
  inorder_traversal(root, increment);

  assert_that(counter, is_equal_to(NUM_OF_NODES));

  avl_dispose(root);
}


Ensure(avl_tree, inorder_traversal_returns_nodes_in_ascending_order)
{
  struct avl_node *root = NULL;

  int i;
  for(i = 0; i < 10; i++)
    root = avl_insert(root, i, NULL);

  old_key = -1;
  inorder_traversal(root, check_ascending_order);

  avl_dispose(root);
}


Ensure(avl_tree, preorder_traversal_goes_through_all_nodes)
{
  struct avl_node *root = tree_constructor();

  counter = 0;
  preorder_traversal(root, increment);

  assert_that(counter, is_equal_to(NUM_OF_NODES));

  avl_dispose(root);
}


Ensure(avl_tree, delete_keeps_the_tree_bst)
{
  struct avl_node *root = tree_constructor();

  int i;
  for(i = 0; i < NUM_OF_NODES; i += 3)
    root = avl_delete(root, i);

  inorder_traversal(root, check_childs);

  avl_dispose(root);
}


Ensure(avl_tree, dispose_works_properly)
{
  struct avl_node *root = tree_constructor();

  avl_dispose(root);

  counter = 0;
  inorder_traversal(root, increment);
  assert_that(counter, is_equal_to(1));
}


static struct avl_node* tree_constructor()
{
  struct avl_node *root = NULL;

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    root = avl_insert(root, i, NULL);

  return root;
}


static void check_childs(struct avl_node *node){
  if(node->height > 2){
    assert_true(abs(node->left->height - node->right->height) <= 1);
    assert_true(node->left->key < node->right->key);
  }
}


static void check_int_data(struct avl_node *node)
{
  assert_that(*(int*)node->data, is_equal_to(INT_DATA));
}


static void increment()
{
  counter += 1;
}


static void check_ascending_order(struct avl_node *node)
{
  assert_true(node->key > old_key);
  old_key = node->key;
}
