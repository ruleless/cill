CC=clang
CXX=clang++
MAKE=make

CFLAGS+=-g -Wall
CXXFLAGS+=-g -Wall

CILLDIR:=../..

CXXFLAGS:=-I $(CILLDIR)
LDFLAGS:=-lpthread -lrt -ldl -L $(CILLDIR)/lib -lcill -lasyncnet

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%.cpp=%.o)
TARGETS:=$(SRCS:%.cpp=%.out)


all:$(TARGETS)

EchoServer.out:EchoServer.o
	$(CXX) -o $@ $^ $(LDFLAGS)

EchoClient.out:EchoClient.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

fake:
	@echo $(OBJS)
