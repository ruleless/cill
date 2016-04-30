CILLDIR:=..

DIRS:=$(CILLDIR)/core $(CILLDIR)/cmd
OBJS:=$(foreach v,$(DIRS),$(wildcard $(v)/*.o))

LIB:=libcill.a
LIBPATH:=$(CILLDIR)/lib

include $(CILLDIR)/link.mak
