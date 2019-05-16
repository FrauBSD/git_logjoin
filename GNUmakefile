# $DruidBSD: /cvsroot/druidbsd/git_logjoin/GNUmakefile,v 1.1 2017/06/21 02:24:18 devinteske Exp $
# $FrauBSD: //github.com/FrauBSD/git_logjoin/GNUmakefile 2019-05-15 21:16:24 -0700 freebsdfrau $

PROG=	git_logjoin
OBJS=	git_logjoin.o string_m.o

CFLAGS=	-Wall -O2

all: $(PROG)

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(^) -o $(@)

.c.o:
	$(CC) $(CFLAGS) -c $(^) -o $(@)

