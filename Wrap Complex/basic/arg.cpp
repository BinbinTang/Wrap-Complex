/* basic/arg.c --- Routines for long command-line arguments */

/*--------------------------------------------------------------------------*/

#include "basic.h"

/*--------------------------------------------------------------------------*/

static int argc;
static char **argv;
static char *buffer;
static int increment = 0;
static string bufferStr;

/*--------------------------------------------------------------------------*/

void basic_arg_open (int arg_c, char **arg_v)
     /* Initialize the arg module. */
{
  int i, l = 0;
  argc = arg_c;
  argv = arg_v;
  for (i=0; i<argc; i++)
    l += (int) strlen (argv[i]) + 1;
  buffer = MALLOC (char, l + 1);
  bufferStr = "";
  HIDE (buffer);
}

/*--------------------------------------------------------------------------*/

void basic_arg_close (void)
     /* Shut down the module. */
{
  argc = 0;
  argv = NULL;
  FREE (buffer);
  increment = 0;
}

/*--------------------------------------------------------------------------*/

char* basic_arg_string (int i)
     /* Copy argv[i..argc] into one string and return pointer to it.
        NOTE: Use STRDUP() if needed. */
{
  // used STL string to carry out the operation instead ro remove the warnings
  if (i < 0) i = 0;
  buffer[0] = '\0';
  while (i < argc)
    {
      bufferStr += argv[i];
	  bufferStr += + " ";
	  //strcat (buffer, argv[i]);
      //strcat (buffer, " ");
      i ++;
    }
  //i = (int) strlen (buffer) - 1;
  //buffer[i] = '\0';  /* clear last blank */
  //return (buffer);
  bufferStr = bufferStr.substr(0, bufferStr.size() - 1);
  return (char*) (bufferStr.c_str());
}

/*--------------------------------------------------------------------------*/

static int match (const char s1[], const char s2[])
{
  return (strcmp (s1, s2) == 0);
}

/*--------------------------------------------------------------------------*/

int basic_arg_match (int i, const char string[])
     /* Matches arg[i] to "string".
        Returns TRUE (!= 0) if successful. */
{
  if (i < argc)
    {
      if (match (argv[i], string))
        {
          increment = 1;
          return (TRUE);
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

int basic_arg_match2int (int i, const char string[], int *val)
     /* Output: val. */
     /* Matches arg[i...] to "string %d" and sets (int) &val accordingly.
        Returns TRUE (!= 0) if successful. */
{
  int i2 = i + 1;
  if (i2 < argc)
    {
      if (match (argv[i], string) and (sscanf_s (argv[i2], "%d", val) == 1))
        {
          increment = 2;
          return (TRUE);
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

int basic_arg_match2float (int i, const char string[], float *val)
     /* Output: val. */
     /* Matches arg[i...] to "string %d" and sets (float) &val accordingly.
        Returns TRUE (!= 0) if successful. */
{
  int i2 = i + 1;
  if (i2 < argc)
    {
      if (match (argv[i], string) and (sscanf_s (argv[i2], "%f", val) == 1))
        {
          increment = 2;
          return (TRUE);
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

int basic_arg_match2double (int i, const char string[], double *val)
     /* Output: val. */
     /* Matches arg[i...] to "string %d" and sets (double) &val accordingly.
        Returns TRUE (!= 0) if successful. */
{
  int i2 = i + 1;
  if (i2 < argc)
    {
      if (match (argv[i], string) and (sscanf_s (argv[i2], "%lf", val) == 1))
        {
          increment = 2;
          return (TRUE);
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

int basic_arg_match2string (int i, const char string[],
                            char buffer[], int length)
     /* Output: buffer[0..length-1]. */
     /* Matches arg[i...] to "string %s" and copies %s into buffer[length].
        Returns TRUE (!= 0) if successful. */
{
  int i2 = i + 1;
  if (i2 < argc)
    {
      if (match (argv[i], string))
        {
		  //strncpy (buffer, argv[i2], length);
		  strncpy_s(buffer, length, argv[i2], length);
          increment = 2;
          return (TRUE);
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

int basic_arg_increment (void)
     /* Returns 1 or 2 if one out of the last batch of basic_arg_match()
        or basic_arg_match2*() calls was successful. Issues a warning if
        returned value is 0. */
{
  if (increment == 0)
    fprintf (stderr, "basic_arg Warning: Zero increment.\n");
  return (increment);
}

/*--------------------------------------------------------------------------*/

int basic_arg_find (const char string[], int arg_c,
                    /*const*/ char *arg_v[])
     /* Returns j > 0 iff one of the arg_v[0..arg_c-1] tokens matches given
        string.  If j > 0, then arg_v[j-1] == string.
        NOTE: this is a stand-alone routine;
        arg module doesn't need to be initialized! */
     /* NOTE:
        The formal parameter declaration  \*const*\ char *arg_v[]  indicates
        that char *arg_v[] is read-only. It seems to me, that the correct ANSI
        C for this should read:  const char * const arg_v[]  ... some compilers
        can't handle this. and issue a warning when the actual parameter is
        not const. (NOTE: NeXT cc seems to know what to do with it!)
        --Ernst, Jul 95. */
{
  int i, j = 0;
  for (i=0; i<arg_c; i++)
    if (match (arg_v[i], string))
      {
        j = i + 1;
        break;
      }
  return (j);
}
