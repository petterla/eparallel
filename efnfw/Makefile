#################################################################
#  Makefile for nfw
#  (c) 2012 Zhang Yongqiang (Beijing, China)
#  Writen by Zhang Yongqiang (efgod@126.com) - 2012-08-14
#################################################################

ROOT = .
BINNAME = libnfw.a

nfw_inc = $(ROOT)
nfw_src = $(ROOT)
be_inc = ../be/
be_src = ../be/

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
IFLAGS1 = -I$(nfw_inc) -I$(be_inc)
CFLAGS = -Wall $(IFLAGS1) 
APPLIBS = 
SYSLIBS = -lpthread
LFLAGS = -L/usr/lib64 -L/usr/local/lib64 -L/usr/lib -L/usr/local/lib 

ifeq ($(MAKECMDGOALS), so)
  CFLAGS = -fPIC -DUNIX $(IFLAGS)
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


cnfs = $(wildcard $(nfw_inc)/*.h)
nfwsources = $(wildcard $(nfw_src)/*.cpp)
besources = $(wildcard $(be_src)/*.cpp)
nfwobjs = $(patsubst $(nfw_src)/%.cpp,$(obj)/%.o,$(nfwsources))
beobjs = $(patsubst $(be_src)/%.cpp,$(obj)/%.o,$(besources))

objs = $(nfwobjs) $(beobjs)

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

$(obj)/%.o: $(nfw_src)/%.cpp 
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(be_src)/%.cpp
	$(COMPILE.c) $< -o $@
