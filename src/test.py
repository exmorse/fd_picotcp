#!/usr/bin/python

from cffi import FFI

ffi = FFI()

ffi.set_source("_fdpicotcp", 
	"""
	#include <fd_picotcp.h>
	""",
        extra_objects=['/usr/lib/libfdpicotcp.so']
	)

#ffi.cdef("int socket (int, int, int);")

ffi.compile()

#Kernel stack socket
s = fdpicotcp.socket(1, 1, 0)
print s

# IPv4 fd_picotcp socket
s = fdpicotcp.socket(1018, 1, 0)
print s
