# $DruidBSD: /cvsroot/druidbsd/git_logjoin/Makefile,v 1.2 2017/06/21 02:39:56 devinteske Exp $
# $FrauBSD: //github.com/FrauBSD/git_logjoin/Makefile 2019-05-15 21:16:24 -0700 freebsdfrau $
# $FreeBSD$

PROG=	git_logjoin
SRCS=	string_m.c git_logjoin.c
CFLAGS=	-I. -I${.CURDIR} -g

MK_MAN=	no

.include <bsd.prog.mk>
