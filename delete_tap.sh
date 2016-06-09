#!/bin/bash 

if [ $# -ne 1 ] 
then
	echo "USAGE: $0 tap_name"
	exit 1
fi

echo "Deleting tap..."

ip link del dev $1 

echo "Done"
