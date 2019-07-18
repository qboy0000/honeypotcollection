#!/bin/sh
# This script runs the valgrind program on honeybrid, to check any lost or leaked memory
honeybrid=$1
config=$2

G_DEBUG=gc-friendly G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=20 --time-stamp=yes --trace-children=yes --track-origins=yes $honeybrid -c $config

exit






logdir=/var/log/honeybrid/valgrind
logfile=valgrind_honeybrid.debug
valgrindfile=valgrind_honeybrid.valgrind
mkdir -p $logdir

cd $logdir

if test -f $logfile
then
                DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$logfile"."$DATE"`
                mv $logfile $BACKUP
                echo "$logfile backed up to $BACKUP"
fi
if test -f $valgrindfile
then
                DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$valgrindfile"."$DATE"`
                mv $valgrindfile $BACKUP
                echo "$valgrindfile backed up to $BACKUP"
fi

cd $workdir

valgrind --tool=memcheck --leak-check=full --time-stamp=yes --trace-children=yes $workdir/honeybrid -c /etc/honeybrid/honeybrid.conf > $logdir/$logfile 2> $logdir/$valgrindfile
