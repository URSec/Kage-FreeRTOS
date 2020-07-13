/*
 * newlib_user.c
 *
 * Unprivileged newlib library
 * Imported from newlib 3.3.0
 *
 *  Created on: Jun 9, 2020
 */
#include <string.h>


void * memsetUser (void *m, int c, size_t n)
{
	char *s = (char *) m;
	while (n--)
	  *s++ = (char) c;

    return m;
}

void * memcpyUser (void *dst0, const void *src0, size_t len0)
{
	char *dst = (char *) dst0;
	char *src = (char *) src0;

	void *save = dst0;

    while (len0--)
    {
      *dst++ = *src++;
    }

    return save;
}

char * strncpyUser (char *dst0, const char *src0, size_t count)
{
	char *dscan;
	const char *sscan;

	dscan = dst0;
	sscan = src0;
	while (count > 0)
	  {
	    --count;
	    if ((*dscan++ = *sscan++) == '\0')
	break;
	  }
	while (count-- > 0)
	  *dscan++ = '\0';

	return dst0;
}
