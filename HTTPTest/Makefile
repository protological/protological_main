#==========================================================
# Makefile of project "HTTPTest"
#==========================================================
TARGET=INTEL

# Basic system (Desktop)
C_COMPILE = gcc
CXX_COMPILE = g++
OBJCOPY=objcopy
RFS_ROOT=

ifeq ($(TARGET),ARM)
    # ARM system
    C_COMPILE = arm-linux-gnueabihf-gcc
    CXX_COMPILE = arm-linux-gnueabihf-g++
    OBJCOPY = arm-linux-gnueabihf-objcopy
    RFS_ROOT=
endif

DFLAGS =
NAME = test
OPT = -O0


# Setup output parameters
PATH_TARGET = 
EXEC = $(NAME).bin
PATH_OBJ    = bin/
CC = $(C_COMPILE)
CXX = $(CXX_COMPILE)
#STRIP = $(C_COMPILE)strip

CFLAGS = -Wall -Wno-unused-function -Wno-unused-variable  -Wno-unused-but-set-variable
CFLAGS += -Wno-pointer-to-int-cast
CFLAGS += -g $(OPT)

CXXFLAGS = -Wno-write-strings -std=c++11 -Wno-format

#DFLAGS += -DDEBUG

# Directories and files
######################################################
DIRS = ./ lib/

# Ignore dir
# https://stackoverflow.com/questions/4210042/exclude-directory-from-find-command
SRC_FILES = $(shell find -L . -name '*.c' -print)
CPP_SRC_FILES = $(shell find -L . -name  '*.cc' -print)
OBJ_FILES = $(subst ./,$(PATH_OBJ),$(patsubst %.c,%.o,$(SRC_FILES)))
OBJ_FILES += $(subst ./,$(PATH_OBJ),$(patsubst %.cc,%.o,$(CPP_SRC_FILES)))

# Some library
######################################################
LIB_PATH = 
LIB_INCLUDE = 
LIB_LIBS = 
LIB_LDFLAGS = 

# Defaults
######################################################
# Need to include headers for libraries
INCLUDE  = -I . 
INCLUDE += -I $(RFS_ROOT)/usr/include/

INCLUDE += $(foreach dir,$(DIRS),-I $(dir))

# Need to link in libs
LIBS = -L .
LIBS += -L $(RFS_ROOT)/usr/lib/

# Use libs
LDFLAGS = 
LDFLAGS += -lrt -lpthread

CFLAGS += 

# Sections and rules
######################################################

# Remove SILENT to see the build cmd
.SILENT:

.PHONY: all all-pre all-post
all: all-pre $(EXEC) all-post

all-pre:
#	echo $(OBJ_FILES)
	echo "-- Building $(EXEC) - $(CC) --"
	mkdir -p $(foreach dir,$(DIRS),$(PATH_OBJ)$(dir))

ifeq ($(TARGET),ARM)
all-post:
	echo "ARM OBJCOPY"
	#$(OBJCOPY) -O binary $(EXEC) $(EXEC).binary
else	
all-post:
	echo "Done..."
endif

$(EXEC):  $(OBJ_FILES)
	echo "  LNK $@"
	$(CC) $(DFLAG) -s -o $(PATH_TARGET)$@ $(foreach dir,$(DIRS),$(PATH_OBJ)$(dir)*.o) $(LIBS) $(LDFLAGS)

$(PATH_OBJ)%.o:%.c
	echo "  CC  $@"
	$(CC) $(CFLAGS) $(INCLUDE) $(DFLAGS) -c -o $@ $<

$(PATH_OBJ)%.o:%.cc
	echo "  CXX $@"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(DFLAGS) -c -o $@ $<

clean:
	echo "  RM .o"
	rm -rf $(PATH_OBJ)
	rm -f $(PATH_TARGET)$(EXEC) 

new: clean all

