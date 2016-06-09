/**
	device.c

	This file contains functions used to manipulate VDE switches, devices and
	interfaces used by fd_picotcp.

	Default values are set, so it is not required use these functions, but they 
	are available to let the uses customize the evironment.

	"set_device_mac()" and "set_switch_path()" need to be called before 
	creating any fd_picotcp socket in order to be effective.
**/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_extern.h"
#include "fd_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Set MAC address of VDE device used by fd_picotcp */
/* In order to be useful this function needs to be called before creating 
   any fd_picotcp sockets, otherwise the default MAC address will be used */
void set_device_mac(unsigned char mac[6]) {
	int i;
	is_mac_changed = 1;
	for (i = 0; i < 6; i++) macaddr[i] = mac[i];
	return;
}


/* Set tap or tun interface name */
/* Stored in the same variable used for vde switch path */
void set_interface_name(char* name) {
	strcpy(vde_switch_name, name);
}


/* Set type of picotcp device to use*/
int set_device_type(int type){
	switch (type) {
		case DEVICE_VDE:
			set_interface_name("/tmp/vde.ctl");
			break;

		case DEVICE_TAP:
			set_interface_name("tap");
			break;

		case DEVICE_TUN:
			set_interface_name("tun");
			break;

		default:
			return -1;
	}
	
	device_type = type;
}


/* Set VDE switch used by fd_picotcp */
/* In order to be useful this function needs to be called before creating 
   any fd_picotcp sockets, otherwise the default path will be used */
int set_switch_path(char* switch_path) {
	strcpy(vde_switch_name, switch_path);
}


/* Set IPv4 netmask used in each interface creation following this call */
/* 'new_mask' is given in string format (e.g. "255.255.0.0") */ 
int set_ipv4_netmask(char* new_mask) {
	strcpy(ipv4_mask_string, new_mask);
} 


/* Set IPv6 netmask used in each interface creation following this call */
/* 'new_mask' is given in string format */ 
int set_ipv6_netmask(char* new_mask) {
	strcpy(ipv6_mask_string, new_mask);
} 
