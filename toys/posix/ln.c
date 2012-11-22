/* ln.c - Create filesystem links
 *
 * Copyright 2012 Andre Renaud <andre@bluewatersys.com>
 *
 * See http://opengroup.org/onlinepubs/9699919799/utilities/ln.html

USE_LN(NEWTOY(ln, "<1nfs", TOYFLAG_BIN))

config LN
  bool "ln"
  default y
  help
    usage: ln [-sf] [FROM...] TO

    Create a link between FROM and TO.
    With only one argument, create link in current directory.

    -s	Create a symbolic link
    -f	Force the creation of the link, even if TO already exists
    -n	Symlink at destination treated as file
*/

#define FOR_ln
#include "toys.h"

void ln_main(void)
{
  char *dest = toys.optargs[--toys.optc], *new;
  struct stat buf;
  int i;

  // With one argument, create link in current directory.
  if (!toys.optc) {
    toys.optc++;
    dest=".";
  }

  // Is destination a directory?
  if (((toys.optflags&FLAG_n) ? lstat : stat)(dest, &buf)
    || !S_ISDIR(buf.st_mode))
  {
    if (toys.optc>1) error_exit("'%s' not a directory");
    buf.st_mode = 0;
  }

  for (i=0; i<toys.optc; i++) {
    int rc;
    char *try = toys.optargs[i];

    if (S_ISDIR(buf.st_mode)) {
      new = strrchr(try, '/');
      if (!new) new = try;
      new = xmsprintf("%s/%s", dest, new);
    } else new = dest;
    /* Silently unlink the existing target. If it doesn't exist,
     * then we just move on */
    if (toys.optflags & FLAG_f) unlink(new);

    rc = (toys.optflags & FLAG_s) ? symlink(try, new) : link(try, new);
    if (rc)
      perror_exit("cannot create %s link from '%s' to '%s'",
        (toys.optflags & FLAG_s) ? "symbolic" : "hard", try, new);
    if (new != dest) free(new);
  }
}