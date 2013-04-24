#################################################################
#  Makefile for vplay server system 
#  (c) 2009 Ma Zhe (Beijing, China)
#  Writen by mazhe (mazhe.zhema@gmail.com) - 2009-07-06
#################################################################

ROOT = .
BINNAME = test 

nfw_inc = ../../efnfw

be_inc = ../../be

main_src = $(ROOT)
main_inc = $(ROOT)

mysql_inc = /usr/include/mysql
mysql_lib = /usr/lib64/mysql

obj = $(ROOT)/obj
dbgobj = $(ROOT)/dbgobj
dst = $(ROOT)/bin

bin = $(dst)/$(BINNAME)


#################################################################
#  Customization of the implicit rules

CC = g++ 
DEFS = -D_DEBUG
IFLAGS1 = -I$(main_inc) -I$(nfw_inc) 
CFLAGS = -Wall $(IFLAGS1) 
APPLIBS = 
SYSLIBS = -lpthread ../../efnfw/bin/libnfw.a
LFLAGS = -L/usr/lib64 -L/usr/local/lib64 -L/usr/lib -L/usr/local/lib
ifeq ($(MAKECMDGOALS), debug)
  CFLAGS = -g  -Wall $(DEFS) $(IFLAGS1) $(IFLAGS2) $(IFLAGS3)
  bin = $(dst)/$(BINNAME).dbg
  APPLIBS = 
endif

LIBS = $(APPLIBS) $(SYSLIBS) 

#################################################################
#  Customization of the implicit rules - BRAIN DAMAGED makes (HP)

AR = ar
ARFLAGS = rv
RANLIB = ranlib
RM = /bin/rm -f
COMPILE.c = $(CC) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS) $(LFLAGS) -o


#################################################################
#  Modules


main_objs = $(obj)/main.o	

objs = $(main_objs)

dbgobjs = $(patsubst $(obj)/%.o,$(dbgobj)/%.o,$(objs))


#################################################################
#  Standard Rules

.PHONY: all clean

all: clean $(bin) 
debug: clean $(bin) 
clean: 
	$(RM) $(objs)
$(bin): $(objs) 
#	@(if ! access -rwx $(obj); then mkdir $(obj);fi)
	$(LINK) $@ $? $(LIBS)

show:
	@(if ! access -rwx $(dbgobj); then mkdir $(dbgobj);fi)
	ls $(objs)

#################################################################
#  Additional Rules


$(obj)/%.o: $(main_src)/%.cpp 
	$(COMPILE.c) $< -o $@

