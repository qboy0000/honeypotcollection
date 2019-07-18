#!/bin/sh

### BEGIN INIT INFO
# Provides: $BIN
### END INIT INFO

BIN=/usr/local/sbin/honeybrid
LOGDIR=/var/log/honeybrid
LOGFILE=honeybrid.log
DEBUGFILE=honeybrid.debug
CONFDIR=/etc/honeybrid
CONF=honeybrid.conf
RUNDIR=/var/run

CONFIG=$CONFDIR/$CONF
SHA1FILE=$CONFDIR/hash_bdd.tb
SOURCEFILE=$CONFDIR/source.tb
LOGFILE=$LOGDIR"/"$LOGFILE
DEBUGFILE=$LOGDIR"/"$DEBUGFILE
PID=$RUNDIR"/honeybrid.pid"

# /etc/init.d/$BIN: start and stop the daemon

test -x $BIN || exit 0

case "$1" in
  start)

# test conf file
        if test -f $CONFIG
        then
		echo "$CONFIG found"
        else
                echo "$CONFIG is missing... can't start"
                exit -1
        fi

# backup log file if exists
        if test -f $LOGFILE
        then
                DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$LOGFILE"."$DATE"`
                mv $LOGFILE $BACKUP
		echo "$LOGFILE backed up to $BACKUP"
        fi
        if test -f $DEBUGFILE
        then
                DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$DEBUGFILE"."$DATE"`
                mv $DEBUGFILE $BACKUP
		echo "$DEBUGFILE backed up to $BACKUP"
        fi
	if test -f $SHA1FILE
	then
		DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$SHA1FILE"."$DATE"`
                cp $SHA1FILE $BACKUP
                echo "$SHA1FILE backed up to $BACKUP"
        fi
        if test -f $SOURCEFILE
        then
                DATE=`date +%Y%m%d_%H%M`
                BACKUP=`echo "$SOURCEFILE"."$DATE"`
                cp $SOURCEFILE $BACKUP
                echo "$SOURCEFILE backed up to $BACKUP"
        fi



        if /sbin/start-stop-daemon --start --quiet --oknodo --pidfile $PID --exec $BIN -- -c $CONFIG; then
            exit 0
        else
            exit 1
        fi
        ;;
  stop)
        echo "Stopping $BIN" 
        if /sbin/start-stop-daemon --stop --quiet --oknodo --pidfile $PID --exec $BIN -- -x `cat $PID`; then
                #echo "killing TCPDUMP recording"
                #kill -9 `ps -edf | grep tcpdump |awk {'print $2'}`
                if test -f $PID; then
                        rm $PID
                fi
                exit 0
        else
            exit 1
        fi
        ;;

  add)
	echo "Adding an IP to the routing and queueing tables"
	if [ $# -gt 1 ];
        then
                LIH_IP=$2
        else
                echo "Please provide the IP address of LIH: "
                read LIH_IP
        fi
	if [ $# -gt 2 ];
        then
                LIH_MAC=$3
        else
                echo "Please provide the MAC address of LIH: "
                read LIH_MAC
        fi
	if [ $# -gt 3 ];
        then
                HIH_IP=$4
        else
                echo "Please provide the IP address of HIH: "
                read HIH_IP
        fi
	if [ $# -gt 4 ];
        then
                EXT_IF=$5
        else
                echo "Please provide the name of the external network interface: "
                read EXT_IF
        fi
	if [ $# -gt 5 ];
        then
                INT_IF=$6
        else
                echo "Please provide the name of the internal network interface: "
                read INT_IF
        fi

	#echo "LIH_IP is: $LIH_IP"
	#echo "LIH_MAC is: $LIH_MAC"
	#echo "HIH_IP is: $HIH_IP"
	#echo "EXT_IF is: $EXT_IF"
	#echo "INT_IF is: $INT_IF"

	echo "Proceeding..."

	route add -host $LIH_IP $INT_IF
	arp -i $EXT_IF -Ds $LIH_IP $EXT_IF pub
	arp -s $LIH_IP $LIH_MAC
	iptables -I FORWARD -i $EXT_IF ! -p icmp -d $LIH_IP -j QUEUE -m comment --comment "honeybrid"
        iptables -I FORWARD -i $INT_IF ! -p icmp -s $LIH_IP -j QUEUE -m comment --comment "honeybrid"
        iptables -I FORWARD -p icmp -s $LIH_IP -j ACCEPT -m comment --comment "honeybrid"
        iptables -I FORWARD -p icmp -d $LIH_IP -j ACCEPT -m comment --comment "honeybrid"
	iptables -I FORWARD -i $INT_IF ! -p icmp -s $HIH_IP -j QUEUE -m comment --comment "honeybrid"
	iptables -I FORWARD -i $INT_IF ! -p icmp -d $HIH_IP -j QUEUE -m comment --comment "honeybrid"

	echo "done."
        exit 0
	;;
  del)
	echo "Removing an IP from the routing and queueing tables"
	if [ $# -gt 1 ];
        then
                LIH_IP=$2
        else
                echo "Please provide the IP address of LIH: "
                read LIH_IP
        fi
	if [ $# -gt 2 ];
        then
                HIH_IP=$3
        else
                echo "Please provide the IP address of HIH: "
                read HIH_IP
        fi
	if [ $# -gt 3 ];
        then
                EXT_IF=$4
        else
                echo "Please provide the name of the external network interface: "
                read EXT_IF
        fi
	if [ $# -gt 4 ];
        then
                INT_IF=$5
        else
                echo "Please provide the name of the internal network interface: "
                read INT_IF
        fi

	echo "Proceeding..."

	route del -host $LIH_IP $INT_IF
	arp -i $EXT_IF -d $LIH_IP
	iptables -D FORWARD -i $EXT_IF ! -p icmp -d $LIH_IP -j QUEUE
        iptables -D FORWARD -i $INT_IF ! -p icmp -s $LIH_IP -j QUEUE
        iptables -D FORWARD -p icmp -s $LIH_IP -j ACCEPT
        iptables -D FORWARD -p icmp -d $LIH_IP -j ACCEPT
	iptables -D FORWARD -i $INT_IF ! -p icmp -s $HIH_IP -j QUEUE
	iptables -D FORWARD -i $INT_IF ! -p icmp -d $HIH_IP -j QUEUE

	echo "done."
        exit 0
	;;

  *)
        echo "Usage: /etc/init.d/$BIN {start|stop|add <lih_ip> <lih_mac> <hih_ip> <ext_if> <int_if>|del <lih_ip> <hih_ip> <ext_if> <int_if>}"
	
        exit 1
esac

exit 0
