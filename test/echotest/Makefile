CC=gcc
CXX=g++
MAKE=make

CFLAGS+=-g -Wall
CXXFLAGS+=-g -Wall

CILLDIR:=../..

CXXFLAGS:=-I$(CILLDIR)/log -I$(CILLDIR) -I$(CILLDIR)/kmem
LDFLAGS:=-L $(CILLDIR)/lib -lcill -llog -lasyncnet -lkmem -lpthread -lrt -ldl

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%.cpp=%.o)
TARGETS:=$(SRCS:%.cpp=%.out)


all:$(TARGETS)

echo_server.out:echo_server.o
	$(CXX) -o $@ $^ $(LDFLAGS)

echo_client.out:echo_client.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	-rm $(OBJS) $(TARGETS)

fake:
	@echo $(OBJS)
