/* basic/iit.c --- Integer interval tree. */

/*
 * n intervals;
 *   specified by their indices e[1..n]
 *   and endpoint-access function:
 *                low  (e[i])
 *                high (e[i])
 *        is_contained (e[i], x)
 *   eg:
 *        interval e[i]          ... "[" low (e[i]) "," high (e[i]) ")"
 *        is_contained (e[i], x) ... (    (low (e[i] <= x)
 *                                    and (x < high (e[i]))
 */


/*--------------------------------------------------------------------------*/

#include "basic.h"

/*--------------------------------------------------------------------------*/

#define MAGIC  130862017  /* Magic number! */

typedef struct node_type
{
  int value;
  int a, b;
  struct node_type *left, *right;
} Node;

/*
 * NOTE: Node allocations has to be done with standard malloc() rather than
 * MALLOC(), because my "malloc-paranoia" package is really designed for (few)
 * dynamic arrays than for many small nodes (hint: FREE() gets *really* slow :)
 */

typedef struct tree_type
{
  int magic;
  Node *root;
  int *sigma;
  int *omega;
  int (* low) (int);
  int (* high) (int);
  int (* is_contained)  (int, int);
  Basic_iit_info info;
} Tree;

static Tree *tree = NULL;
static int path_nodes;

static Node* make_node      (int i, int j);
static Node* new_node       (void);
static void select_value    (int *index, int *value, int i, int j);
static void query_procedure (const Node *node, int x, void (* report) (int));
static void node_kill       (Node *node);
static int path_length      (const Node *node, int depth);
static int sigma_compare    (const int *i, const int *j);
static int omega_compare    (const int *i, const int *j);

#ifdef __DEBUG__
 static int is_valid_input  (int i, int j);
 static int is_valid_output (const Node *node, int i, int j);
 static int is_right_split  (int i, int j, int r, int value);
 static int is_sorted       (const int array[], int i, int j,
                             int (* endpoint) (int));   
 static int is_empty        (const int array[], int i, int j);
#endif

int basic_iit_proto_flag = FALSE;  /* Exported flag! */

#define Proto  if (basic_iit_proto_flag) printf /* ... */

/*--------------------------------------------------------------------------*/

Basic_iit basic_iit_build (int n, int e[],
                           int (* low) (int), int (* high) (int),
                           int (* is_contained) (int, int))
     /* Input/Output: e[1..n]. */
     /* Builds interval tree given as mentioned above. */
{
  double time;
  ;
  tree = MALLOC (Tree, 1);
  tree->magic = MAGIC;
  tree->low = low;
  tree->high = high;
  tree->is_contained = is_contained;
  tree->sigma = e;
  tree->omega = MALLOC (int, n + 1);
  BZERO (tree->omega,   int, n + 1);
  ;
  BZERO (&(tree->info), Basic_iit_info, 1);
  tree->info.n = n;
  ;
  /* sigma[1..n] = e[1..n], sorted wrt. low() */
  time = basic_utime ();
  basic_qsort (tree->sigma, 1, n, sigma_compare);
  tree->info.lowsort_time = basic_utime () - time;
  ;
  /* make first node, and recurse... */
  time = basic_utime ();
  tree->root = make_node (1, n);
  tree->info.buildup_time = basic_utime () - time;
  ;
  return ((Basic_iit) tree);
}

/*--------------------------------------------------------------------------*/

static Node* make_node (int i, int j)
     /* Makes node out of tree->sigma[i..j], and recurses. */
{
  Assert (is_valid_input (i, j));
  if (i > j)
    return ((Node *) NULL);
  else
    {
      Node *node = new_node ();
      int lambda, iota;
      int q, r;
      ;
      /* select value & get "right of" intervals sigma[r+1..j] */
      select_value (&r, &(node->value), i, j);
      ;
      /* mark "contains" intervals in sigma[i..r] to omega[i<q+1..r] */
      q = r;
      for (lambda=r; lambda>=i; lambda--)
        if (tree->is_contained (node->value, tree->sigma[lambda]))
          {
            tree->omega[q] = tree->sigma[lambda];
            tree->sigma[lambda] = 0;
            q --;
          }
      ;
      /* move remaining "left of" intervals from sigma[i..r] to sigma[i..q] */
      iota = i;
      for (lambda=i; lambda<=r; lambda++)
        if (tree->sigma[lambda] != 0)
          {
            tree->sigma[iota] = tree->sigma[lambda];
            iota ++;
          }
      Assert (iota == q + 1);
      ;
      /* copy omega[q+1..r] back to sigma[q+1..r] & sort omega[q+1..r] */
      for (lambda=q + 1; lambda<=r; lambda++)
        {
          tree->sigma[lambda] = tree->omega[lambda];
        }
      basic_qsort (tree->omega, q + 1, r, omega_compare);
      node->a = q + 1;
      node->b = r;
      ;
#if 0
      printf (" NODE=%d [%d..%d], left: %d, cont: %d, right: %d\n",
             node->value, i, j, q - i + 1, r - q, j - r);
#endif
      ;
      Assert (is_valid_output (node, i, j));
      ;
      /* recurse */
      node->left  = make_node (i, q);
      node->right = make_node (r + 1, j);
      ;
      return (node);
    }
}

/*--------------------------------------------------------------------------*/

static Node *new_node (void)
{
  tree->info.nodes ++;
  return ((Node *) malloc (sizeof (Node)));  /* Node allocation! */
}

/*--------------------------------------------------------------------------*/

static void select_value (int *index, int *value, int i, int j)
     /* Output: index, value. */
     /* Refinement of make_node().
        Select proper value and split off "right of" triangles. */
{
  int r = j - (j - i) / 3;
  int k = tree->low (tree->sigma[r]);
  ;
  while ((r < j) and (tree->low (tree->sigma[r + 1]) == k))
    r ++;
  ;
  if (not (tree->is_contained (k, tree->sigma[r])))
    { /* adjust r to the left for "open" intervals */
      while ((r > i) and (not (tree->is_contained (k, tree->sigma[r - 1]))))
        {
          r --;
          printf (" basic_iit: (-)\n");
        }
      if (not tree->is_contained (k, tree->sigma[r]))
        { 
          r --;
          printf (" basic_iit: [-]\n");
          Assert_always (r == i - 1);
          printf (" basic_iit WARNING: empty NODE!?!\n");
        }
    }
  Assert (is_right_split (i, j, r, k));
  *index = r;
  *value = k;
}

/*--------------------------------------------------------------------------*/

void basic_iit_query (Basic_iit t, int x, void (* report) (int))
     /* Query ... */
{
  double time = 0.0;
  if (basic_iit_proto_flag)
    time = basic_utime ();
  Assert_always (t and (((Tree *) t)->magic == MAGIC));
  tree = (Tree *) t;
  Proto ("<<basic_iit_query: ");
  query_procedure (tree->root, x, report);
  Proto ("= %.2fs>>\n", basic_utime () - time);
}

static void query_procedure (const Node *node, int x, void (* report) (int))
{
  int lambda;
  if (node == NULL)
    return;
  else if (x == node->value)
    {
      Proto ("%dD ", node->value);
      for (lambda=node->a; lambda<=node->b; lambda++)
        report (tree->sigma[lambda]);
    }
  else if (x < node->value)
    {
      Proto ("%dL ", node->value);
      query_procedure (node->left, x, report);
      for (lambda=node->a; lambda<=node->b; lambda++)
        {
          if (tree->is_contained (x, tree->sigma[lambda]))
            report (tree->sigma[lambda]);
          else
            return;
        }
      return;
    }
  else /* (node->value < x) */
    {
      Proto ("%dR ", node->value);
      query_procedure (node->right, x, report);
      for (lambda=node->b; lambda>=node->a; lambda--)
        {
          if (tree->is_contained (x, tree->omega[lambda]))
            report (tree->omega[lambda]);
          else
            return;
        }
      return;
    }
}

/*--------------------------------------------------------------------------*/

void basic_iit_kill (Basic_iit t)
     /* Input/Output; t. */
{
  if (t)
    {
      Tree *tree = (Tree *) t;
      Assert_always (tree->magic == MAGIC);
      FREE (tree->sigma);
      FREE (tree->omega);
      node_kill (tree->root);
      FREE (tree);
    }
}

static void node_kill (Node *node)
{
  if (node)
    {
      node_kill (node->left);
      node_kill (node->right);
      free (node);  /* Node de-allocation! */
    }
}

/*--------------------------------------------------------------------------*/

Basic_iit_info* basic_iit_info (const Basic_iit t)
{
  Basic_iit_info *inf;
  Assert_always (t and (((Tree *) t)->magic == MAGIC));
  tree = (Tree *) t;
  inf = &(tree->info);
  inf->bytes_array = (inf->n + 1) * (int) sizeof (int) * 2;
  inf->bytes_tree = (inf->nodes) * (int) sizeof (Node);
  inf->bytes = (int) sizeof (Tree) + inf->bytes_tree + inf->bytes_array;
  path_nodes = 0;
  inf->max_depth = 0;
  inf->filled_depth = MAXINT;
  inf->avg_depth = (double) path_length (tree->root, 1) / inf->n;
  inf->avg_nodesize = (double) inf->n / inf->nodes;
  Assert_always (path_nodes == inf->nodes);
  return (inf);
}

static int path_length (const Node *node, int depth)
{
  if (node == NULL)
    {
      if (depth  < tree->info.filled_depth)
        tree->info.filled_depth = depth - 1;  /* side effect! */
      return (0);
    }
  else
    {
      path_nodes ++;
      if (depth > tree->info.max_depth)
        tree->info.max_depth = depth;  /* side effect! */
      return (  depth * (node->b - node->a + 1)
              + path_length (node->left,  depth + 1)
              + path_length (node->right, depth + 1));
    }
}

/*--------------------------------------------------------------------------*/

static int sigma_compare (const int *i, const int *j)
     /* Compare tree->sigma[i],[j] wrt. tree->low(); cf: basic_iit_build(). */
{
  int a = tree->low (*i);
  int b = tree->low (*j);
  if (a == b)
    return (0);
  else if (a < b)
    return (-1);
  else
    return (1);
}

static int omega_compare (const int *i, const int *j)
     /* Compare tree->omega[i],[j] wrt. tree->high(); cf: make_node(). */
{
  int a = tree->high (*i);
  int b = tree->high (*j);
  if (a == b)
    return (0);
  else if (a < b)
    return (-1);
  else
    return (1);
}

/*--------------------------------------------------------------------------*/

#ifdef __DEBUG__

static int is_valid_input (int i, int j)
{
  int lambda, iota;
  Assert (is_sorted (tree->sigma, i, j, tree->low));
//  Assert (is_empty (tree->omega, i, j));
  for (lambda=i; lambda<=j; lambda++)
    {
      iota = tree->sigma[lambda];
      Assert (   tree->is_contained (tree->low  (iota), iota)
              or tree->is_contained (tree->high (iota), iota));
    }
  return (TRUE);
}

static int is_valid_output (const Node *node, int i, int j)
{
  int lambda;
  ;
  Assert ((i <= node->a) and (node->b <= j));
  Assert (is_sorted (tree->sigma, node->a, node->b, tree->low));
  Assert (is_sorted (tree->omega, node->a, node->b, tree->high));
  Assert (is_right_split (i, j, node->b, node->value));
  ;
  for (lambda=node->a; lambda<=node->b; lambda++)
    Assert (    tree->is_contained (node->value, tree->sigma[lambda])
            and tree->is_contained (node->value, tree->omega[lambda]));
  ;
  for (lambda=i; lambda<node->a; lambda++)
    Assert (not tree->is_contained (node->value, tree->sigma[lambda]));
  for (lambda=node->b + 1; lambda<=j; lambda++)
    Assert (not tree->is_contained (node->value, tree->sigma[lambda]));
  ;
  return (TRUE);
}

static int is_right_split (int i, int j, int r, int value)
{
  int iota, lambda;
  iota = 0;
  for (lambda=i; lambda<=j; lambda++)
    if (tree->is_contained (value, tree->sigma[lambda]))
      iota = lambda;
  return (iota == r);
}  

static int is_sorted (const int array[], int i, int j,
                      int (* endpoint) (int))
     /* Assume: array[i..j]. */
{
  int lambda;
  for (lambda=i; lambda<j; lambda++)
    if (endpoint (array[lambda]) > endpoint (array[lambda + 1]))
      return (FALSE);
  return (TRUE);
}

static int is_empty (const int array[], int i, int j)
{
  int lambda;
  for (lambda=i; lambda<=j; lambda++)
    if (array[lambda] != 0)
      return (FALSE);
  return (TRUE);
}

#endif
