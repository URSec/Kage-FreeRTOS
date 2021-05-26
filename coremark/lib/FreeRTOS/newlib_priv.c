/*
 * newlib_priv.c
 *
 * Privileged newlib library
 * Imported from newlib 3.3.0
 *
 * Created on: Jun 9, 2020
 */
#include <string.h>


void * memsetPriv (void *m, int c, size_t n) __attribute__((section("privileged_functions")))
{
	char *s = (char *) m;
	while (n--)
	  *s++ = c;

    return m;
}

// void * memcpy (void *dst0, const void *src0, size_t len0)
// {
// 	char *dst = (char *) dst0;
// 	char *src = (char *) src0;

// 	void *save = dst0;

//     while (len0--)
//     {
//       *dst++ = *src++;
//     }

//     return save;
// }

// char * strncpy (char *dst0, const char *src0, size_t count)
// {
// 	char *dscan;
// 	const char *sscan;

// 	dscan = dst0;
// 	sscan = src0;
// 	while (count > 0)
// 	  {
// 	    --count;
// 	    if ((*dscan++ = *sscan++) == '\0')
// 	break;
// 	  }
// 	while (count-- > 0)
// 	  *dscan++ = '\0';

// 	return dst0;
// }
