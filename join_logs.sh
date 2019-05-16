#!/bin/sh
#-
# Copyright (c) 2013-2019 Devin Teske <dteske@FreeBSD.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
############################################################ IDENT(1)
#
# $Title: Test program to merge git log histories $
# $DruidBSD: /cvsroot/druidbsd/git_logjoin/join_logs.sh,v 1.2 2017/06/21 02:39:56 devinteske Exp $
# $FrauBSD: //github.com/FrauBSD/git_logjoin/join_logs.sh 2019-05-15 21:21:28 -0700 freebsdfrau $
# $FreeBSD$
#
############################################################ CONFIGURATION

#
# Directory to look for logs in
#
LOGDIR=logs

#
# List of git logfiles to join
# NOTE: Format is local_repos_dir:local_log_file
#
LOGS="
	$HOME/src/fraubsd/pkgcenter:$LOGDIR/pkgcenter.log

" # END-QUOTE

#
# File(s) to save output(s) to
#
OUTPUT=../fraubsd.log

#
# Should we use sysutils/pv to monitor the progress? (recommended)
# Set to the NULL string to disable the use of pv(1).
#
USE_PV=YES

############################################################ FUNCTIONS

have() { type "$@" > /dev/null 2>&1; }

############################################################ MAIN

#
# Make sure we have git_logjoin
#
if [ ! -e git_logjoin ]; then
	echo ">>> Building git_logjoin utility..."
	make || exit 1
fi

#
# First make sure we have each log
#
mkdir -p "$LOGDIR" || exit 1
for log in $LOGS; do
	
	logfile="${log#*:}"
	[ -e "$logfile" ] && continue

	# Download the log history
	repos="${log%%:*}"
	echo ">>> Downloading \`$repos' History to \`$logfile'..."
	if [ "$USE_PV" ] && have pv; then
		( cd "$repos" && git pull > /dev/null && \
			git log --pretty=format:user:%aN%n%ct --reverse --raw \
				--encoding=UTF-8 --no-renames ) |
			pv > "$logfile"
	else
		[ "$USE_PV" ] && echo "NOTE: If you install sysutils/pv" \
		                      "you'll get a lot more feedback!"
		( cd "$repos" && git pull > /dev/null && \
			git log --pretty=format:user:%aN%n%ct --reverse --raw \
				--encoding=UTF-8 --no-renames ) > "$logfile"
	fi
	[ $? -eq 0 ] || exit 1
done

#
# Combine the logfiles into a single output
#
echo ">>> Combining logfiles into \`$OUTPUT':"
if [ "$USE_PV" ] && have pv; then
	# Get a list of sizes so we can tell pv how many bytes to expect
	bytes=0
	for log in $LOGS; do
		logfile="${log#*:}"
		# Subtract 52 bytes for non-duplicated data
		[ $bytes -ne 0 ] && bytes=$(( $bytes - 52 ))
		case "${UNAME_s:=$( uname -s )}" in
		Linux) bytes=$(( $bytes + $( stat -c%s "$logfile" ) )) ;;
		    *) bytes=$(( $bytes + $( stat -f%z "$logfile" ) ))
		esac
	done
	# Combine the logs using pv(1) to monitor the flow of data
	# NOTE: pv(1) provides us with its own running/total time
	echo "NOTE: Percentage expected to be slightly over 100% given changes"
	./git_logjoin $LOGS | pv -s $bytes > "$OUTPUT"
else
	[ "$USE_PV" ] && echo "NOTE: If you install sysutils/pv" \
	                      "you'll get a lot more feedback!"
	# Combine the logs (this may take a while and not give any feedback)
	time ./git_logjoin $LOGS > "$OUTPUT"
fi

################################################################################
# END
################################################################################
