TOPDIR:=..

DIRS:=$(TOPDIR)/core $(TOPDIR)/cmd
OBJS:=$(foreach v,$(DIRS),$(wildcard $(v)/*.o))

LIB:=libcill.a
LIBPATH:=$(TOPDIR)/lib

include ./build.mak
