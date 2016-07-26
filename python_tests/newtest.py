#!/usr/bin/python

from cffi import FFI

ffi = FFI()
ffi.cdef("int socket(int family, int type, int proto);")

fdpicotcp = ffi.dlopen("../shared_lib/libfdpicotcp.so")

#Kernel stack socket
s = fdpicotcp.socket(1, 1, 0)
print "Kernel Socket - fd: " + str(s)

#Kernel stack socket
s = fdpicotcp.socket(1, 1, 0)
print "Kernel Socket - fd: " + str(s)

# IPv4 fd_picotcp socket
s = fdpicotcp.socket(1018, 1, 0)
print "fdpicotcp Socket - fd : " + str(s)

# IPv4 fd_picotcp socket
s = fdpicotcp.socket(1018, 1, 0)
print "fdpicotcp Socket - fd : " + str(s)
