CILLDIR:=../..

CXXFLAGS:=-D_USE_KMEM -I$(CILLDIR) -I$(CILLDIR)/kmem -I$(CILLDIR)/log
LDFLAGS:=-L $(CILLDIR)/lib -lkmem -llog -lpthread

TARGET:=main.out

include $(CILLDIR)/build.mak
