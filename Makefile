# Filename:             Makefile
# Date:                 11/20/2020
# Author:               Karin Nielsen
# Version:              1.0
# Copyright:            2020, All Rights Reserved
#
# Description:
#	Makefile for text-based GUI using Curses / CDK

#compiler, compiler flags, and preprocessor flags for implicit rules
CXX = g++
CXXFLAGS = -Wall -Wno-write-strings #-str=c++11
CPPFLAGS = -I /scratch/perkins/include

#linker flag
LDFLAGS = -L /scratch/perkins/lib
# linker library flags; cdk wraps around curses so call it first
LDLIBS = -lcdk -lcurses

#project name for backup target
PROJECTNAME = assignment6

#executable name
EXEC = program6

#define files needed to build executable (source, headers, and object files)
SRCS = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

# These targets don't create files that Make should inspect
.PHONY: all clean backup

#default target
all: $(EXEC)

# remove unnecessary files
clean:
	rm -f $(OBJS) *.d *.d* *~ $(EXEC) \#*

# We use the preprocessor to create "dependency" files.  The dependency files include
# all header files that any given .cc file #includes. The idea is: if any header file 
# changes, we need to recompile all the .cc files that included the header file.

# Pattern for .d files.
%.d:%.cc
	@echo Updating .d Dependency File
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

#links the files
$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(OBJS) $(LDFLAGS) $(LDLIBS)
	@echo Linking $(OBJS) into $(EXEC)

# Read the man page to learn how a make handles a target named Makefile.  This allows 
# make to "restart" if it had to change (or create) any .d files.  The "restart" 
# allows make to re-read the dependency files after that were created or changed.
Makefile: $(SRCS:.cc=.d)

#backup
backup: clean
	@mkdir -p /home/010/k/ka/kan140330/backups; chmod 700 /home/010/k/ka/kan140330/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := /home/010/k/ka/kan140330/backups/$(PROJECTNAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing Backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!

# Include the dependency files created by the PreProcessor. The dash in front of 
# the command keeps the system from complaining if the files do not exist. This 
# rule is used in conjunction with the "Makefile" target above.
-include $(SRCS:.cc=.d)