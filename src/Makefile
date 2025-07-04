#
# Tools
#

GCC		= gcc
CPP		= g++
CXX      = $(CPP)

# use common.c and common.h
COMMON_LIB := libcommon.a
COMMON_DIR := ./
LDFLAGS += -L$(COMMON_DIR)       # Suchpfad, falls nötig
LDLIBS  += 

# Build values

LD       = $(CXX)
AR       = ar

RM		 = rm -f

INC 	+= -I. -I$(COMMON_DIR) -I/usr/local/include

CFLAGS		= $(INC) $(DOCTEST) -fmax-errors=4 -Wno-unused-result -Wno-unused-function
CPPFLAGS	= $(INC) $(DOCTEST) -std=c++11 -g
CXXFLAGS	= $(INC) $(DOCTEST) -std=c++11 -g

# Paths
LD_LIBRARY_PATH = -L$(LIBDIR) -Wl,-rpath=$(LIBDIR)

LDFLAGS	= -lpthread -lzmqpp -lzmq -lsodium -Wl,--no-as-needed
SOURCES	= $(wildcard *.cpp)
TARGETS	= $(SOURCES:%.cpp=%)

ifeq ("$(DEBUG)","1")
	CFLAGS += -Wall -O0 -Wunused
else
	CFLAGS += -Wall -O2 -D DOCTEST_CONFIG_DISABLE #-D DEBUG=0
endif


.phony: all clean

common:
	$(GCC) $(INC) $(CFLAGS) -c common.c -o libcommon.a
	@echo "Generated libcommon, the oldschool C low level helper converions..."
	@echo "Example: gcc program.c libcommon.a -o program"
	$( shell ln -s -T libcommon.a libcommon.so )
	
all: common ${TARGETS}

clean:
	${RM} *.obj *~* ${TARGETS}
	$(RM) libcommon.a libcommon.so

${TARGETS}:
	@echo "Inheriting libraries $(LDFLAGS)"
	@echo "Include path: $(INC)"
	${CPP}  ${CPPFLAGS} -o $@ $< ${@:%=%.cpp} ${LDFLAGS} $(LDLIBS)


