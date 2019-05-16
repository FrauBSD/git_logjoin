/*-
 * Copyright (c) 2001-2013 Devin Teske <dteske@FreeBSD.org>
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
 * $DruidBSD: /cvsroot/druidbsd/git_logjoin/string_m.h,v 1.2 2017/06/21 02:39:56 devinteske Exp $
 * $FreeBSD: head/lib/libfigpar/string_m.h 274116 2014-11-04 23:46:01Z dteske $
 * $FrauBSD: //github.com/FrauBSD/git_logjoin/string_m.h 2019-05-15 21:16:24 -0700 freebsdfrau $
 */

#ifndef _STRING_M_H_
#define _STRING_M_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
int             replaceall(char *source, const char *find,
                    const char *replace);
unsigned int    strcount(const char *source, const char *find);

#ifdef __cplusplus
}
#endif

#endif /* !_STRING_M_H_ */
