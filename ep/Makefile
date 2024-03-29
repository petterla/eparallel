#################################################################
#  Makefile for nfw
#  (c) 2012 Zhang Yongqiang (Beijing, China)
#  Writen by Zhang Yongqiang (efgod@126.com) - 2012-08-14
#################################################################

ROOT = .
BINNAME = eparallel.a

ep_inc = $(ROOT)
ep_src = $(ROOT)
be_inc = ../be
be_src = ../be

obj = $(ROOT)/obj
dbgobj = $(ROOT)/dbgobj
dst = $(ROOT)/bin
libdst = $(ROOT)/bin

bin = $(dst)/$(BINNAME)
solib = $(ROOT)/bin


#################################################################
#  Customization of the implicit rules

CC = g++ 
DEFS = -D_DEBUG
IFLAGS = -I$(ep_inc) -I$(be_inc) 
CFLAGS = -Wall $(IFLAGS) 
APPLIBS = 
SYSLIBS = -lpthread ../be/bin/libbe.a
LFLAGS = -L/usr/lib64 -L/usr/local/lib64 -L/usr/lib -L/usr/local/lib 

ifeq ($(MAKECMDGOALS), so)
  CFLAGS = -fPIC -DUNIX $(IFLAGS)
  LIBS = $(SYSLIBS)
endif

ifeq ($(MAKECMDGOALS), debug)
  CFLAGS = -g -Wall -DUNIX $(DEFS) $(IFLAGS)
  LIBS = $(SYSLIBS)
endif


#################################################################
#  Customization of the implicit rules - BRAIN DAMAGED makes (HP)

AR = ar
ARFLAGS = rv
RANLIB = ranlib
RM = /bin/rm -f
COMPILE.c = $(CC) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS) $(LFLAGS) -o
SOLINK = $(CC) $(CFLAGS) $(LFLAGS) -shared -o


#################################################################
#  Modules


cnfs = $(wildcard $(ep_inc)/*.h)
sources = $(wildcard $(ep_src)/*.cpp)
objs = $(patsubst $(ep_src)/%.cpp,$(obj)/%.o,$(sources))
dbgobjs = $(patsubst $(ep_src)/%.cpp,$(dbgobj)/%.o,$(sources))
sobin = $(patsubst %.a,%.so,$(bin))


#################################################################
#  Standard Rules

.PHONY: all so clean debug show

all: clean $(bin)
so: clean $(sobin)
debug: clean $(bin)
clean: 
	$(RM) $(objs)
show:
	@echo $(bin)
	ls $(objs)

#################################################################
#  Additional Rules

$(sobin): $(objs) 
	$(SOLINK) $@ $? 

$(bin): $(objs) 
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $(RANLIBFLAGS) $@

$(obj)/%.o: $(ep_src)/%.cpp 
	$(COMPILE.c) $< -o $@ 
