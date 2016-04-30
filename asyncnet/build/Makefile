ROOTDIR:=../..
ASYNCDIR:=..

DIRS:=$(ASYNCDIR) $(ASYNCDIR)/src
OBJS:=$(foreach v,$(DIRS),$(wildcard $(v)/*.o))

LIB:=libasyncnet.a
LIBPATH:=$(ROOTDIR)/lib

include $(ROOTDIR)/link.mak
