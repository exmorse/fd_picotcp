#!/bin/bash 

if [ $# -ne 2 ] 
then
	echo "USAGE: $0 tap_name tap_address"
	exit 1
fi

echo "Creating tap..."

ip tuntap add name $1 mode tap
ip addr add ${2}/24 dev $1
ip link set dev $1 up

echo "Done"
