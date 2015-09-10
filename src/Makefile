# CS165 MakeFile
# This file includes automatic dependency tracking!
# Please see example of "utils" to see how to add additional files
# to your project.

all: client server

# C-compiler settings
CC = gcc -std=c99 -g -ggdb3

# Default optimization level
O ?= 3

# Flags and other libraries
CFLAGS = -Wall -Wextra -pedantic -pthread -O$(O) -I$(INCLUDES)
LDFLAGS =
LIBS =
INCLUDES = include

####### Automatic dependency magic #######
# Set-up dependency directory
DEPSDIR := .deps
BUILDSTAMP := $(DEPSDIR)/rebuildstamp
DEPFILES := $(wildcard $(DEPSDIR)/*.d)
ifneq ($(DEPFILES),)
include $(DEPFILES)
endif
DEPCFLAGS = -MD -MF $(DEPSDIR)/$*.d -MP

# Dependency compilation
ifneq ($(DEP_CC),$(CC) $(CFLAGS) $(DEPCFLAGS) $(O))
DEP_CC := $(shell mkdir -p $(DEPSDIR); echo >$(BUILDSTAMP); echo "DEP_CC:=$(CC) $(CFLAGS) $(DEPCFLAGS) $(O)" >$(DEPSDIR)/_cc.d)
endif

# Make sure dependency directories are generated
$(DEPSDIR)/stamp $(BUILDSTAMP):
	mkdir -p $(@D)
	touch $@

####### Automatic dependency magic #######

%.o : %.c $(BUILDSTAMP)
	$(CC) $(CFLAGS) $(DEPCFLAGS) -O$(O) -o $@ -c $<

##
# To include additional non-executable files (e.g. selects.c), you'll
# need to add an additional build rule here. See "utils" as an example,
# and you should be able to just copy the two lines and change the name.
#
# After that, you should include it as a "dependency" on any other builds
# that require it.  For example, if you had just added "utils" and then
# client and server depends on it, you would add "utils.o" to the right
# side of the build rule
##

utils: utils.o
	$(CC) $(CFLAGS) $(DEPCFLAGS) -c -o $@ $<

client: client.o utils.o
	$(CC) $(CFLAGS) $(DEPCFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

server: server.o utils.o
	$(CC) $(CFLAGS) $(DEPCFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f client server *.o *~ *.bak core *.core cs165_unix_socket
	rm -rf .deps

distclean: clean
	rm -rf $(DEPSDIR)

.PHONY: all clean distclean
