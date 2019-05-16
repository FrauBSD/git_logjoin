/*-
 * Copyright (c) 2013-2019 Devin Teske <dteske@FreeBSD.org>
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
 * $DruidBSD: /cvsroot/druidbsd/git_logjoin/git_logjoin.c,v 1.2 2017/06/21 02:39:56 devinteske Exp $
 */

#include <sys/cdefs.h>
#ifdef __FBSDID
__FBSDID("$FreeBSD$");
__FBSDID("$FrauBSD: //github.com/FrauBSD/git_logjoin/git_logjoin.c 2019-05-15 21:16:24 -0700 freebsdfrau $");
#endif

#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#ifndef ARG_MAX
#define ARG_MAX 2048
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux
#define __USE_XOPEN
#endif
#include <time.h>

#include "string_m.h"

#define FILE_ENTRY_FOOTER	""
#define FILE_ENTRY_HEADER	"user:"
#define FILE_ENTRY_HEADER_LEN	5
#define FILE_DATE_FORMAT_ERROR	"%s: Invalid epoch"

struct log_entry
{
	uint8_t hold;
	struct tm date;
	char *paths;
	char *user;
	size_t paths_len;
	size_t user_len;
};

struct log_file
{
	char *repos, *path;
	FILE *file;
	struct log_entry entry;
};

static char *gbuf = NULL;
static size_t gbuf_len = 0;

/* Function prototypes for private functions (see style(9)) */
void	gbuf_xrealloc(size_t len);
void	paths_xrealloc(struct log_entry *entry, size_t len);
char *	read_one(struct log_file *log, size_t *llen);
void	xrealloc(void **ptr, size_t newlen, size_t *curlen);

int
main(int argc, char *argv[])
{
	uint8_t failed = 0;
	uint8_t print_sep = 0;
	int n, completed;
	size_t len;
	struct log_file logs[ARG_MAX], *log;
	struct log_entry *entry;

	/* We need at least 1 positional argument */
	if (argc < 2)
		errx(EXIT_FAILURE, "too few arguments");

	/* Attempt to open each/every log file */
	for (n = 0; n < argc - 1; n++)
	{
		log = &logs[n];

		/* Parse the repos name and logfile path from argument */
		log->repos = argv[n+1];
		log->path = strchr(argv[n+1], ':');
		if (log->path == NULL)
			errx(EXIT_FAILURE, "%s: Need `repos:logfile'",
			     argv[n+1]);
		else
			*(log->path++) = '\0';

		/* open the logfile */
		if ((log->file = fopen(log->path, "r")) == NULL) {
			failed = 1;
			break;
		}
	}
	if (failed) {
		warn("%s", log->path);
		/* Close previously opened log files */
		for (; --n >= 0;) fclose(logs[n].file);
		return EXIT_FAILURE;
	}

	fprintf(stderr, "All logs opened successfully!\n");

	/*
	 * Initialize the output
	 */
	for (n = 0; n < argc - 1; n++) {
		log = &logs[n];
		log->entry.paths = NULL;
		log->entry.paths_len = 0;
		log->entry.hold = 0;
	}

	/*
	 * Loop until we've read all logentries from all logfiles
	 */
	for (;;)
	{
		time_t time0, time1;
		struct log_file *oldest;
		char seconds[21];

		completed = 0;
		for (n = 0; n < argc - 1; n++)
			if (logs[n].file == NULL) completed++;
		if (completed == argc - 1) break;

		/* Fill the ranks */
		for (n = 0; n < argc - 1; n++)
		{
			log = &logs[n];
			/* Skip logs that are closed and done */
			if (log->file == NULL) continue;

			entry = &(log->entry);
			/* Skip logs that have a held entry already */
			if (entry->hold) continue;

			if (read_one(log, &len) == NULL) {
				fclose(log->file);
				log->file = NULL;
				continue;
			}
		}

		/* Find the log with the [current] oldest entry */
		oldest = NULL;
		for (n = 0; n < argc - 1; n++)
		{
			log = &logs[n];
			/* Skip logs that are closed and done */
			if (log->file == NULL) continue;

			entry = &(log->entry);
			entry->hold = 1;
			if (oldest == NULL) {
				oldest = log;
				continue;
			}
			time0 = mktime(&(oldest->entry.date));
			time1 = mktime(&(entry->date));
			if (difftime(time1, time0) < 0) {
				/* new oldest entry found */
				oldest = log;
			}
		}

		/* No logfile entries left */
		if (oldest == NULL) break;

		log = oldest;
		entry = &(log->entry);

		/* Add the repository name */
		gbuf_xrealloc(strlen(log->repos) + 3);
		sprintf(gbuf, "\t%s/", log->repos);
		len = strlen(entry->paths);
		len += strcount(entry->paths, "\t") * (strlen(gbuf) - 1);
		paths_xrealloc(entry, len);
		if (replaceall(entry->paths, "\t", gbuf) < 0)
			err(EXIT_FAILURE, "%s: replaceall()",
			    __func__);

		if (print_sep) printf("\n");
		printf("%s%s\n", FILE_ENTRY_HEADER, entry->user);
		strftime(seconds, 20, "%s", &(entry->date));
		printf("%s\n", seconds);
		printf("%s\n", entry->paths);

		entry->hold = 0;
		print_sep = 1;
	}

	/* Close all the log files */
	for (n = 0; n < argc - 1; n++) {
		log = &logs[n];
		if (log->file == NULL) continue;
		fclose(log->file);
	}

	return EXIT_SUCCESS;
}

char *
read_one(struct log_file *log, size_t *llen)
{
	uint8_t entry_start = 0, entry_end = 0;
	size_t len;
#ifdef __linux
	size_t size = 0;
#endif
	char *line;
	char *retval = NULL;
	struct log_entry *entry = &(log->entry);

	/* Find and stash the user */
	while (!entry_start)
	{
#ifdef __linux
		len = (size_t)getline(&line, &size, log->file);
#else
		line = fgetln(log->file, &len);
#endif
		if (len <= 0 || line[len-1] != '\n') break;
		gbuf_xrealloc(len);
		snprintf(gbuf, len, "%s", line);
		if (strncmp(gbuf, FILE_ENTRY_HEADER, FILE_ENTRY_HEADER_LEN)
			!= 0) continue;
		entry_start = 1;
	}
	if (!entry_start) goto read_one_fail;
	xrealloc((void **)&(entry->user),
		len - FILE_ENTRY_HEADER_LEN + 1, &(entry->user_len));
	snprintf(entry->user, entry->user_len,
		"%s", (gbuf + FILE_ENTRY_HEADER_LEN));
	
	/* Get the epoch */
#ifdef __linux
	len = (size_t)getline(&line, &size, log->file);
#else
	line = fgetln(log->file, &len);
#endif
	if (len <= 0 || line[len-1] != '\n') goto read_one_fail;
	retval = line;
	if (strptime(line, "%s", &(entry->date)) == NULL)
		errx(EXIT_FAILURE, FILE_DATE_FORMAT_ERROR, line);

	/* Initialize paths */
	paths_xrealloc(entry, 1);
	entry->paths[0] = '\0';

	/* Determine how long the entry is by reading up to the footer */
	while (!entry_end)
	{
#ifdef __linux
		len = (size_t)getline(&line, &size, log->file);
#else
		line = fgetln(log->file, &len);
#endif
		if (len <= 0 || line[len-1] != '\n') break;
		retval = line;
		gbuf_xrealloc(len);
		snprintf(gbuf, len, "%s", line);
		if (strncmp(gbuf, FILE_ENTRY_HEADER, FILE_ENTRY_HEADER_LEN)
			== 0)
		{
			paths_xrealloc(entry, 1);
			entry->paths[0] = '\0';
			xrealloc((void **)&(entry->user),
				len - FILE_ENTRY_HEADER_LEN + 1,
				&(entry->user_len));
			snprintf(entry->user, entry->user_len,
				"%s", (gbuf + FILE_ENTRY_HEADER_LEN));
#ifdef __linux
			len = (size_t)getline(&line, &size, log->file);
#else
			line = fgetln(log->file, &len);
#endif
			if (len <= 0 || line[len-1] != '\n')
				goto read_one_fail;
			retval = line;
			gbuf_xrealloc(len);
			snprintf(gbuf, len, "%s", line);
			if (strptime(gbuf, "%s", &(entry->date)) == NULL)
				errx(EXIT_FAILURE,
					FILE_DATE_FORMAT_ERROR, gbuf);
			continue;
		}
		else if (strcmp(gbuf, FILE_ENTRY_FOOTER) == 0) {
			entry_end = 1;
			break;
		}
		paths_xrealloc(entry, strlen(entry->paths) + len + 1);
		if (entry->paths[0] != '\0') strcat(entry->paths, "\n");
		strcat(entry->paths, gbuf);
	}

	*llen = len; /* pass back length of last line */
	return retval; /* return pointer to last line */

read_one_fail:
	if (entry->paths != NULL) entry->paths[0] = '\0';

	*llen = 0;
	return retval;
}

void
xrealloc(void **ptr, size_t newlen, size_t *curlen)
{
	if (newlen <= *curlen) return;
	if ((*ptr = realloc(*ptr, newlen)) == NULL)
		errx(EXIT_FAILURE, "realloc: out of memory?!");
	*curlen = newlen;
}

void
gbuf_xrealloc(size_t len)
{
	xrealloc((void **)&gbuf, len, &gbuf_len);
}

void
paths_xrealloc(struct log_entry *entry, size_t len)
{
	xrealloc((void **)&(entry->paths), len, &(entry->paths_len));
}
