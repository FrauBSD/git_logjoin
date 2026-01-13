/*-
 * Copyright (c) 2001-2026 Devin Teske <dteske@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $DruidBSD: /cvsroot/druidbsd/git_logjoin/string_m.c,v 1.2 2017/06/21 02:39:56 devinteske Exp $
 */

#include <sys/cdefs.h>
#ifdef __FBSDID
__FBSDID("$FreeBSD: head/lib/libfigpar/string_m.c 274116 2014-11-04 23:46:01Z dteske $");
__FBSDID("$FrauBSD: //github.com/FrauBSD/git_logjoin/string_m.c 2026-01-13 10:07:46 -0800 freebsdfrau $");
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "string_m.h"

/*
 * Counts the number of occurrences of one string that appear in the source
 * string. Return value is the total count.
 *
 * An example use would be if you need to know how large a block of memory
 * needs to be for a replaceall() series.
 */
unsigned int
strcount(const char *source, const char *find)
{
        unsigned int n = 0;
        size_t flen;
        const char *p = source;

        /* Both parameters are required */
        if (source == NULL || find == NULL) return 0;

        /* Cache the length of find element */
        flen = strlen(find);
        if (strlen(source) == 0 || flen == 0) return 0;

        /* Loop until the end of the string */
        while (*p != '\0')
        {
                if (strncmp(p, find, flen) == 0) {
                        p += flen; n++; /* found an instance */
                } else
                        p++;
        }

        return n;
}

/*
 * Replaces all occurrences of `find' in `source' with `replace'.
 *
 * You should not pass a string constant as the first parameter, it needs to be
 * a pointer to an allocated block of memory. The block of memory that source
 * points to should be large enough to hold the result. If the length of the
 * replacement string is greater than the length of the find string, the result
 * will be larger than the original source string. To allocate enough space for
 * the result, use the function strcount() declared above to determine the
 * number of occurrences and how much larger the block size needs to be.
 *
 * If source is not large enough, the application will crash. The return value
 * is the length (in bytes) of the result.
 *
 * When an error occurs, -1 is returned and the global variable errno is set
 * accordingly. Returns zero on success.
 */
int
replaceall(char *source, const char *find, const char *replace)
{
        size_t slen, flen, rlen;
        uint32_t n = 0;
        char *p, *t, *temp;

        errno = 0; /* reset global error number */

        /* Check that we have non-null parameters */
        if (source == NULL) return 0;
        if (find == NULL) return strlen(source);

        /* Cache the length of the strings */
        slen = strlen(source);
        flen = strlen(find);
        rlen = replace ? strlen(replace) : 0;

        /* Cases where no replacements need to be made */
        if (slen == 0 || flen == 0 || slen < flen) return slen;

        /* If replace is longer than find, we'll need to create a temp copy */
        if (rlen > flen) {
                temp = malloc(slen + 1);
                if (errno) return -1; /* could not allocate memory */
                snprintf(temp, slen, "%s", source);
        } else
                temp = source;

        /* Reconstruct the string with the replacements */
        p = source; t = temp; /* position elements */

        while (*t != '\0')
        {
                if (strncmp(t, find, flen) == 0) {
                        /* found an occurrence */
                        for (n = 0; replace && replace[n]; n++)
                                *p++ = replace[n];
                        t += flen;
                } else
                        *p++ = *t++; /* copy character and increment */
        }

        /* Terminate the string */
        *p = 0;

        /* Free the temporary allocated memory */
        if (temp != source) free(temp);

        /* Return the length of the completed string */
        return strlen(source);
}
