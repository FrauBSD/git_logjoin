[//]: # ($FrauBSD: //github.com/FrauBSD/git_logjoin/README.md 2019-05-15 21:31:52 -0700 freebsdfrau $)

# Welcome to [FrauBSD.org/git\_logjoin](https://fraubsd.org/git_logjoin)!

Join history from multiple git repositories into a single log

## Foreword

The following is required before using `git commit` in this project.

> `$ .git-hooks/install.sh`

This will ensure the FrauBSD keyword is expanded/updated for each commit.

## Introduction

So you have multiple Git repositories that you work on. Let's say that you
would like to view (in chronological order) a history of your work. There are
tools such as Andrew Caudwell's excellent Gource.

Unfortunately, although Gource does support rendering logs produced directly
from Git (using the below commands to generate your log):

        cd my-git-project
        git log > my-project.log
        gource my-project.log

Your currently stuck if you'd like to concurrently render multiple
repositories.

Enter `git_logjoin` (this tool).

I whipped this up extremely fast (2 days) to solve the problem.

There's no C-code to edit, only shell.

## How to use:

### 1. Modify `join_logs.sh` to fit your needs. Primarily:

	Change `LOGS=""` to contain appropriate repos:logfile entries
	NOTE: Currently the shell script assumes all repos are local paths

### 2. [OPTIONAL] Install sysutils/pv from the ports tree (provides I/O feedback):

#### For current releases using PkgNG:

    pkg add pv

#### For current releases using legacy pkg_tools:

    pkg_add -r pv

#### For outdated releases using legacy pkg\_tools:

    env `PACKAGESITE=ftp://ftp-archive.freebsd.org/pub/FreeBSD-Archive/ports/`uname -p`/packages-X.Y-release/Latest/ pkg_add -r pv`

\# NOTE: replace X.Y with appropriate values
\# NOTE: First and second line join to form `FreeBSD-Archive'

Of course, you could always use the ports tree if you don't want a binary package.

### 3. Run `./join_logs.sh`

If all is said and done properly, the `join_logs.sh` script will first
acquire the logs for you and then combine them into a single log.

You then pass the combined-log to a tool like Gource.

### 4. Re-running `join_logs.sh`

If you remove any of the log files that were previously acquired, simply
re-run `join_logs.sh` and it will generate new ones with the latest revision
   history. The process is designed to (once you have configured `join_logs.sh`)
   allow you to run the script each time you want an up-to-date log file to
   feed to gource.

## Notes

The code could be optimized for code comments, slightly smaller memory
footprint, and perhaps parallelization (though that would be tricky).

Designed to work on half-gigabyte sized log files from git 1.8.3.4.

Untested with any format of git log other than the ones produced by:

    git log --pretty=format:user:%aN%n%ct --reverse --raw \
                --encoding=UTF-8 --no-renames

This is the format required by gource.

## Sample output

Below is a sample output from running `join_logs.sh' without modification:

NOTE: sysutils/pv was installed before collecting the following output.

```
git_logjoin $ ./join_logs.sh
>>> Building git_logjoin utility...
Warning: Object directory not changed from original /data/scratch/git_logjoin
cc -I. -I/data/scratch/git_logjoin -g -std=gnu99 -fstack-protector  -c string_m.c
cc -I. -I/data/scratch/git_logjoin -g -std=gnu99 -fstack-protector  -c git_logjoin.c
cc -I. -I/data/scratch/git_logjoin -g -std=gnu99 -fstack-protector   -o git_logjoin string_m.o git_logjoin.o 
>>> Git repository server: git@somehost
>>> Downloading `dir/repos1.git' History to `logs/repos1.log'...
19.5MB 0:00:14 [1.32MB/s] [              <=>                                   ]
>>> Downloading `dir/repos2.git' History to `logs/repos2.log'...
 168MB 0:01:54 [1.47MB/s] [              <=>                                   ]
>>> Combining logfiles into `logs/join_logs.log':
NOTE: Percentage expected to be slightly over 100% given changes
All logs opened successfully!
 188MB 0:15:42 [ 572kB/s] [===================================] 101%            

END.
```

