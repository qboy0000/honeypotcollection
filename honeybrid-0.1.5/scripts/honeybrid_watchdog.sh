#! /bin/sh

echo 'starting watchdog for honeybrid'

while [ 1 ]
do
        pidof -s honeybrid > /dev/null

        if [ $? == 0 ]
        then
                echo ! > /dev/null
        else
                /etc/init.d/honeybrid.sh stop
                /etc/init.d/honeybrid.sh start
        fi

        sleep 60
done
