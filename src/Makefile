GXX				= gcc
CFLAGS			= -I. $(DOCTEST) -fmax-errors=4 -Wno-unused-result -Wno-unused-function
CPP			    = g++
CPPFLAGS		= -I. $(DOCTEST) -std=c++11 -g

RM			    = rm -f


INC += -I./

LDFLAGS			= -lpthread -lzmqpp -lzmq -lsodium -Wl,--no-as-needed
SOURCES			= $(wildcard *.cpp)
TARGETS			= $(SOURCES:%.cpp=%)

# use common.c and common.h
COMMON_LIB := libcommon.a
COMMON_DIR := ./
LDFLAGS += -L$(COMMON_DIR)       # Suchpfad, falls nötig
LDLIBS  += -lcommon              # verlinkt libcommon.a

ifeq ("$(DEBUG)","1")
	CFLAGS += -Wall -O0 -Wunused
else
	CFLAGS += -Wall -O2 -D DOCTEST_CONFIG_DISABLE #-D DEBUG=0
endif


.phony: all clean

common:
	$(GXX) $(INC) $(CFLAGS) -c common.c -o libcommon.a
	@echo "Generated libcommon, the oldschool C low level helper converions..."
	@echo "Example: gcc program.c libcommon.a -o program"
	
all: common ${TARGETS}

clean:
	${RM} *.obj *~* ${TARGETS}
	$(RM) libcommon.a

${TARGETS}:
	@echo "Inheriting libraries $(LDFLAGS)"
	@echo "Include path: $(INC)"
	${CPP}  ${CPPFLAGS} -o $@ $< ${@:%=%.cpp} ${LDFLAGS} $(LDLIBS)


