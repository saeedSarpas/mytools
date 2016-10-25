/*
 * avltree/avl_tree.c
 * test_file: avltree/avl_tree_test.c
 *
 * AVL tree module
 * AVL is a self-balancing binary search tree. The complexity of insertion and
 * deletion in this tree is O(logn) which makes it a good data stucture for
 * large data sets
 *
 * NOTE: this module has been written mostly with the help of the following
 * tutorial:
 * http://www.geeksforgeeks.org/avl-tree-set-1-insertion/
 *
 * author: Saeed Sarpas
 */


#include <stdarg.h>
#include "avl_tree.h"
#include "./../memory/allocate.h"
#include "./../memory/copy.h"


static int max(int, int);
static int height(struct avl_node*);
static int get_balance(struct avl_node*);
static struct avl_node* right_rotate(struct avl_node*);
static struct avl_node* left_rotate(struct avl_node*);
static struct avl_node* min_node(struct avl_node*);
static struct avl_node* delete_node(struct avl_node*);


/*
 * finding a node using its key
 *
 * param: root the root node
 * param: key an unique integer key
 */
struct avl_node* avl_find(struct avl_node *root, int64_t key)
{
  if(root == NULL) return root;

  if(key < root->key)
    root = avl_find(root->left, key);
  else if(key > root->key)
    root = avl_find(root->right, key);

  return root;
}


/*
 * creating a new node
 *
 * param: key an unique integer key
 * param: data data to be assigned to the node
 */
static struct avl_node* new_node(int64_t key, void *data, int nmemb, int size)
{
  struct avl_node* node = allocate(1, sizeof(struct avl_node));

  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;

  if(data != NULL){
    node->data = allocate(nmemb,size);
    copy(data, node->data, nmemb * size);
  }

  return node;
}


/*
 * inserting a node with a specific key into the tree (the key should be unique)
 *
 * param: node the root node of the tree
 * param: key the unique key for the new node to be inserted
 * param: data the data to attach to the node. Note that if data is not null,
 * one should also pass two integer after it, nmemb with is the number of the
 * member of the allocated space for node->data and size which the size of each
 * member of the allocated space
 *
 * TODO: currently insert only accepts integer keys
 */
struct avl_node* avl_insert(struct avl_node *node, int64_t key, void *data, ...)
{
  int nmemb = 0, size = 0;
  if(data != NULL){
    va_list a_list;
    va_start(a_list, data);
    nmemb = va_arg (a_list, int);
    size = va_arg (a_list, int);
    va_end(a_list);
  }

  if(node == NULL) return new_node(key, data, nmemb, size);

  if(key < node->key)
    node->left = avl_insert(node->left, key, data, nmemb, size);
  else if(key > node->key)
    node->right = avl_insert(node->right, key, data, nmemb, size);
  else
    return node;

  // Update height of the parents
  node->height = max(height(node->left), height(node->right)) + 1;

  // Checking the balance factor to see whether this node became unbalanced
  int balance = get_balance(node);

  // If this node becomes unbalanced, then there are 4 cases
  // Left Left Case
  if(balance > 1 && key < node->left->key)
    return right_rotate(node);

  // Right Right Case
  if(balance < -1 && key > node->right->key)
    return left_rotate(node);

  // Left Right Case
  if(balance > 1 && key > node->left->key){
    node->left =  left_rotate(node->left);
    return right_rotate(node);
  }

  // Right Left Case
  if(balance < -1 && key < node->right->key){
    node->right = right_rotate(node->right);
    return left_rotate(node);
  }

  // return the (unchanged) node pointer
  return node;
}


/*
 * deleting node with a specific key from the tree
 *
 * param: root the root node of the tree
 * param: key the key of the node to be deleted
 *
 * TODO: currently delete only accepts integer keys
 */
struct avl_node *avl_delete(struct avl_node *root, int64_t key)
{
  // Normal BST delete
  if(root == NULL) return root;

  if(key < root->key)
    root->left = avl_delete(root->left, key);
  else if(key > root->key)
    root->right = avl_delete(root->right, key);
  else{
    root = delete_node(root);
  }

  // If the tree had only one node then return
  if(root == NULL) return root;

  // Update height of the parents
  root->height = max(height(root->left), height(root->right)) + 1;

  // Checking the balance factor to see whether this node became unbalanced
  int balance = get_balance(root);

  // If this node becomes unbalanced, then there are 4 cases
  // Left Left Case
  if(balance > 1 && get_balance(root->left) >= 0)
    return right_rotate(root);

  // Left Right Case
  if(balance > 1 && get_balance(root->left) < 0){
    root->left =  left_rotate(root->left);
    return right_rotate(root);
  }

  // Right Right Case
  if(balance < -1 && get_balance(root->right) <= 0)
    return left_rotate(root);

  // Right Left Case
  if(balance < -1 && get_balance(root->right) > 0){
    root->right = right_rotate(root->right);
    return left_rotate(root);
  }

  return root;
}


/*
 * Traversing through the tree in an ascending order
 *
 * param: node the root node
 * param: func pointer to a function for operating on each node
 */
void inorder_traversal(struct avl_node *node, void (*func)(struct avl_node*))
{
  if(node == NULL)
    return;

  inorder_traversal(node->left, func);
  func(node);
  inorder_traversal(node->right, func);
}


/*
 * Traversing throught the tree in a way to meet all parents before their childs
 *
 * param: node the root node
 * param: func pointer to a function for operating on each node
 */
void preorder_traversal(struct avl_node *node, void (*func)(struct avl_node*))
{
  if(node == NULL)
    return;

  func(node);
  preorder_traversal(node->left, func);
  preorder_traversal(node->right, func);
}


/*
 * Disposing the tree
 *
 * param: node the root node
 */
struct avl_node* avl_dispose(struct avl_node *root)
{
  struct avl_node *min;
  while(root != NULL){
    min = min_node(root);
    root = avl_delete(root, min->key);
  }

  return root;
}


/*
 * comparing two integers and returning the maximum
 *
 * param: a, b two given integers
 */
static int max(int a, int b)
{
  return a > b ? a : b;
}


/*
 * returning the height of the node
 *
 * param: node a given avl node
 */
static int height(struct avl_node *node)
{
  return node == NULL ? 0 : node->height;
}


/*
 * returning the balance of a given node
 *
 * param: node a given node
 */
static int get_balance(struct avl_node *node)
{
  return node == NULL ? 0 : height(node->left) - height(node->right);
}


/*
 * rotating the tree around a given node clockwise
 *
 * param: node
 */
static struct avl_node *right_rotate(struct avl_node *y)
{
  struct avl_node *x = y->left;
  struct avl_node *T2 = x->right;

  // Perform rotation
  x->right = y;
  y->left = T2;

  // Update heights
  y->height = max(height(y->left), height(y->right))+1;
  x->height = max(height(x->left), height(x->right))+1;

  // Return new root
  return x;
}


/*
 * rotating the tree around a given node anticlockwise
 *
 * param: node
 */
static struct avl_node *left_rotate(struct avl_node *x)
{
  struct avl_node *y = x->right;
  struct avl_node *T2 = y->left;

  // Perform rotation
  y->left = x;
  x->right = T2;

  //  Update heights
  x->height = max(height(x->left), height(x->right))+1;
  y->height = max(height(y->left), height(y->right))+1;

  // Return new root
  return y;
}


/*
 * Find the minimum of the tree
 *
 * param: node
 */
static struct avl_node *min_node(struct avl_node *node)
{
  struct avl_node *current = node;

  /* loop down to find the leftmost leaf */
  while(current->left != NULL)
    current = current->left;

  return current;
}


/*
 * Delete a given node
 *
 * param: node
 */
static struct avl_node* delete_node(struct avl_node *node)
{
  if(node->data != NULL)
    free(node->data);

  // node with only one child or no child
  if((node->left == NULL) || (node->right == NULL)){
    struct avl_node *temp = node->left ? node->left : node->right;

    // No child case
    if(temp == NULL){
      temp = node;
      node = NULL;
    } else {
      // One child case
      *node = *temp;
    }

    free(temp);
    return node;
  } else {
    // node with two children: Get the inorder successor (smallest
    // in the right subtree)
    struct avl_node* temp = min_node(node->right);

    // Copy the inorder successor's data to this node
    node->key = temp->key;

    // Delete the inorder successor
    node->right = avl_delete(node->right, temp->key);
  }

  return node;
}
