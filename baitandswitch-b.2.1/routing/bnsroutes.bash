#!/bin/bash 


ext_nic=eth0	   
ext_ip=12.109.94.230	   
prod_nic=eth1   
prod_gw=192.168.1.1     
honey_nic=eth2 
honey_gw=192.168.2.1   
multi_ip=192.168.1.2   


ip rule add from 192.168.1.2/24 table my_out
ip rule add to 192.168.1.2/32 table production
ip rule add fwmark 1 table honeypot
ip route add 192.168.1.2/32 via 192.168.1.1 dev eth1 table production proto static
ip route add 192.168.1.2/32 via 192.168.2.1 dev eth2 table honeypot proto static
ip route add 0/0 via 12.109.94.230 dev eth0
