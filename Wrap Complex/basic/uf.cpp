/* uf.c -- Union-Find Data Structure
 *
 * Date -- 10/4/94 separated from volbl.c by Mike
 * Date -- 7/10/95 converted to ANSI C by Ernst
 */

/*-----------------------------------------------------------------------*/

#include "basic.h"

/*-----------------------------------------------------------------------*/

#define NIL -1
#define Switch(i, j) { int tmp; tmp = i; i = j; j = tmp; }

static int num_elts = 0;
static int num_sets = 0;
static Basic_uf_record *ufelts;        /* Union/Find structure. */

/*-----------------------------------------------------------------------*/

#if 0
static int size_compare (int i, int j);
#endif

/*-----------------------------------------------------------------------*/

void basic_uf_set (Basic_set_queue *sq)
{ /* set num_elts, num_sets, ufelts according to sq, so that other finds
     and unions can be performed.  Note:  there are still some problems
     (mainly memory issues) that need to be resolved before this can
     be used effectively.                                                */
  
  num_elts = sq->num_elts;
  num_sets = sq->num_sets;
  ufelts = sq->ufelts;
}

/*--------------------------------------------------------------------------*/

void basic_uf_create (int n)
{ /* union-find array [1..n], it is a parallel array to the t_hash_table.*/

  ufelts = MALLOC(Basic_uf_record, n);  /* n is the size of t_hash_table */

  num_elts = n;
  num_sets = 0;
  BZERO(ufelts, Basic_uf_record, n);
}

/*--------------------------------------------------------------------------*/

int basic_uf_find (int i)
{ /* Given an element index, i, find the set to which it belongs.        */
  int s;
  s = ufelts[i].set;
  return If(s<0, i, s);       /* return the root of i                  */
}

/*--------------------------------------------------------------------------*/

void basic_uf_add (int i)
{ /* Add a new set.                                                      */
    ufelts[i].set = -1;          /* element i defines a set by itself    */
    ufelts[i].next = NIL;        /* no next element                      */
    num_sets++;                  /* increment number of sets             */
}

/*--------------------------------------------------------------------------*/

void basic_uf_delete (int i)
{ /* Removes item i from the UF structure.  NOTE:  This is an expensive
     O(n) time operation currently.                                      */

  int cur;
  int new_set;

  Assert_always (ufelts[i].next);

  if (ufelts[i].set < 0)                                  /* i is a root */
    if (-ufelts[i].set == 1) {                /* only one element in set */
      ufelts[i].set = ufelts[i].next = 0;
      num_sets--;
    }
    else {                               /* more than one element in set */
      new_set = ufelts[i].next;
      ufelts[ufelts[i].next].set = ufelts[i].set + 1;
      ufelts[i].set = ufelts[i].next = 0;
      cur = ufelts[new_set].next;
      while (cur > 0)                    /* update sets for all elements */
        {
          ufelts[cur].set = new_set;
          cur = ufelts[cur].next;
        }
    }
  else /* i is not a root, and must be located in the list to be removed */
    {
      cur = ufelts[i].set;
      ufelts[cur].set += 1;
      while (ufelts[cur].next != i)
        cur = ufelts[cur].next;
      ufelts[cur].next = ufelts[ufelts[cur].next].next;
      
      ufelts[i].set = ufelts[i].next = 0;
    }
}

/*--------------------------------------------------------------------------*/

void basic_uf_union (int i, int j)
{ /* Given sets i and j, form the union of the two sets.                 */
int p;

    Assert(ufelts[i].next);     /* make sure i, j exist                  */
    Assert(ufelts[j].next);

    i = basic_uf_find(i);       /* find the root of i                    */
    j = basic_uf_find(j);       /* find the root of j                    */
    if (i != j) {               /* if different sets                     */
        num_sets--;             /* decrement number of sets              */
        if (((ufelts[i].set     /* contains -1 * number of elts in set i */
          >                     /* means number of elts <                */
        ufelts[j].set)          /* contains -1 * number of elts in set j */
          and
        i != 0)                 /* set 0 is a special set and need       */
          or                    /* to stay set 0                         */
        j == 0)
        Switch(i, j);           /* i must be the larger set              */
                                /* the special treatment of set 0 does   */
                                /* not upset the nlogn time complexity   */
        ufelts[i].set += ufelts[j].set;
        p = j;                  /* set p to the smaller set              */
        while (ufelts[p].next != NIL) {
            ufelts[p].set = i;  /* every element in small set moves to   */
                                /* the large set, except the last        */
            p = ufelts[p].next;
        }
        ufelts[p].set = i;      /* move last element of the small        */
                                /* set to the larger set                 */
        ufelts[p].next = ufelts[i].next; /* it points now to the element */
                                        /* that is right after the root  */
        ufelts[i].next = j;             /* the next after the root is    */
                                        /* the first of the small set    */
    }
}

/* ^   *What* is this??
   |   It certainly doesn't win a price in readability. :) :)
   |   In any case, I fully parenthesised the condition to silent gcc -Wall.
   |    --Ernst. */
   
/*--------------------------------------------------------------------------*/

Basic_set_queue* basic_uf_collect (void)
{ /* Computes number of elements for each set, and sorts the sets by     */
  /* size                                                                */

  /* NOTE: previously called sort_by_volume */
  /*       The variable num_sets is the number of sets including the     */
  /*       0 set.  This is excluded in nsets below, since some           */
  /*       applications of UF don't use a 0 set.                         */

  Assert_always (0);
  return 0;

#if 0
  int i, set = 0;
  int *root_list;
  Basic_set_queue *sq = MALLOC (Basic_set_queue, 1);
  int nsets = (ufelts[0].set ? (num_sets-1) : num_sets);

  if (nsets > 0)
    {
      root_list = MALLOC (int, nsets + 1);          /* roots begin at 1. */

      for (i=1; i<=num_elts; i++)
	if (ufelts[i].set < 0) {  /* Root of a set is a negative number. */
	  root_list[++set] = i;
#ifdef PRINT_UF
	  printf ("Adding set %d: %d\n", set, i);
#endif
	}
    }
  else
    root_list = MALLOC (int, 1);

  Assert_always (set == nsets);

  basic_heapsort (root_list, nsets, size_compare);

  root_list[0] = 0;  

  sq->num_elts = num_elts;
  sq->num_sets = nsets;
  sq->ufelts = ufelts;
  sq->root_list = root_list;

  return (sq);
#endif
}

/*--------------------------------------------------------------------------*/

#if 0
static int size_compare (int i, int j)
{ /* Compares the sizes of sets i and j, returning -1, 0, or +1          */

  int ans;

  if (ufelts[i].set < ufelts[j].set)
    ans = 1;
  else if (ufelts[i].set == ufelts[j].set)
    ans = 0;
  else
    ans = -1;

#ifdef PRINT_UF
  printf ("Comparing %d (%d) and %d (%d) : %d\n\n", i, -ufelts[i].set,
         j, -ufelts[j].set, ans);
#endif
  return ans;
}
#endif

/*--------------------------------------------------------------------------*/

int basic_uf_set_size (int i)
{ /* Returns the size of the set containing i */

  if (ufelts[i].set > 0)
    i = ufelts[i].set;

  return (- ufelts[i].set);
}

/*--------------------------------------------------------------------------*/

int basic_uf_num_sets (void)
{ /* Returns the number of sets */
  /* The 0 set is not included in this number. */

  return (ufelts[0].set ? (num_sets-1) : num_sets);
}

/*--------------------------------------------------------------------------*/

void basic_uf_free (Basic_set_queue *sq)
{ /* Deallocate memory for set queue */

  FREE (sq->ufelts);
  FREE (sq->root_list);
  FREE (sq);
}

/*--------------------------------------------------------------------------*/

void basic_uf_print (void)
{ /* Print the current state of the UF structure */

  int i, cur;

  printf ("\n\nThe UF Structure: %d sets\n\n", num_sets);
  for(i=1; i<=num_elts; i++)
    if (ufelts[i].set < 0)
      {
        printf("Root: %d (size %d), ", i, -ufelts[i].set);
        cur = ufelts[i].next;
        while (cur > 0) {
          printf ("%d ", cur);
          cur = ufelts[cur].next;
        }
        printf("\n");
      }
}
