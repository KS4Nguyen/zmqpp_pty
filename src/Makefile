#
# Tools
#
APP_STL := c++_shared

GCC		= gcc
CPP		= g++
CXX      = $(CPP)

SHELL := /bin/bash
.SHELLFLAGS := -o pipefail -c

# use common.c and common.h
COMMON_LIB := libcommon.a
COMMON_DIR := common/
LDFLAGS += -L$(COMMON_DIR)

# use pty.c and pty.h
PTY_LIB := libpty.a
PTY_DOR := pty/
LDFLAGS += -L$(PTY_DIR)

LDLIBS  += 

# Build values

LD       = $(CXX)
AR       = ar

RM		 = rm -f

INC 	+= -I. -I$(COMMON_DIR) -I$(PTY_DIR) -I/usr/local/include

CFLAGS		= $(INC) $(DOCTEST) -fmax-errors=4 -Wno-unused-result -Wno-unused-function
CPPFLAGS	= $(INC) $(DOCTEST) -std=c++11 -g \
	-fno-diagnostics-show-caret \
	-fno-diagnostics-show-template-tree \
	-ftemplate-backtrace-limit=0 \
	-fno-diagnostics-show-option \
	-fmax-errors=2

CXXFLAGS	= $(INC) $(DOCTEST) -std=c++11 -g

# Paths
LD_LIBRARY_PATH = -L$(LIBDIR) -Wl,-rpath=$(LIBDIR)

LDFLAGS	+= -lpthread -lzmqpp -lzmq -lsodium -Wl,--no-as-needed
SOURCES	:= $(wildcard *.cpp)
TARGETS	 = $(SOURCES:%.cpp=%)

ifeq ("$(DEBUG)","1")
	CFLAGS += -Wall -O0 -Wunused
else
	CFLAGS += -Wall -O2 -D DOCTEST_CONFIG_DISABLE #-D DEBUG=0
endif


.phony: all clean

common:
	$(GCC) $(CFLAGS) -c common/common.c -o libcommon.a
	@echo "Generated libcommon, the oldschool C low level helper converions..."
	@echo "Example: gcc program.c libcommon.a -o program"
	$( shell ln -s -T libcommon.a libcommon.so )

pty:
	$(GCC)  $(CFLAGS) -c pty/pty.c -o libpty.a
	@echo "Generated libpty"
	$( shell ln -s -T libpty.a libpty.so )
	
all: common ${TARGETS}

clean:
	${RM} *.obj *~* ${TARGETS}
	$(RM) libcommon.a libcommon.so
	$(RM) libpty.a libpty.so

${TARGETS}:
	@echo "Inheriting libraries $(LDFLAGS)"
	@echo "Include path: $(INC)"
	${CPP} $(INC) ${CPPFLAGS} -o $@ $< ${@:%=%.cpp} ${LDFLAGS} $(LDLIBS) 2>&1 \
	| sed '/: note:/d' # | grep --color=always -E 'error'




