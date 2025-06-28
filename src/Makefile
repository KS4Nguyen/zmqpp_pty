DEBUG=1
ifeq ($(DEBUG),1)
   BUILD_MODE=debug
endif

CPP			    = g++
RM			    = rm -f
CPPFLAGS		= -Wall -I. -O2 -std=c++11 -g
LDFLAGS			= -lpthread -lzmqpp -lzmq -lsodium -Wl,--no-as-needed
SOURCES			= $(wildcard *.cpp)
TARGETS			= $(SOURCES:%.cpp=%)

ifeq ("$(DEBUG)","1")
	CFLAGS += -Wall -Wextra -O0 -Wunused
else
	CFLAGS += -Wall -Wextra -O2 -D DOCTEST_CONFIG_DISABLE #-D DEBUG=0
endif

all:	${TARGETS}

clean:
	${RM} *.obj *~* ${TARGETS}

${TARGETS}:
	${CPP} ${CPPFLAGS} -o $@ ${@:%=%.cpp} ${LDFLAGS}

