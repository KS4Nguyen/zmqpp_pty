#
# Instance values, command line user specifiable
#

CONFIG   = max
CPPFLAGS = -Wall -O2 -std=c++11 -g
CXXFLAGS =
LDFLAGS  = -lpthread -lzmqpp -lzmq -lsodium -Wl,--no-as-needed

PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin
LIBDIR = $(DESTDIR)$(PREFIX)/lib
PKGCONFIGDIR = $(LIBDIR)/pkgconfig
INCLUDEDIR = $(DESTDIR)$(PREFIX)/include

#
# Tools
#

#CXX      = g++
LD       = $(CXX)
AR       = ar

#
# Build values
#

LIBRARY_NAME     = zmqpp
VERSION_MAJOR    = 4
VERSION_MINOR    = 2
VERSION_REVISION = 0

#
# Paths
#

LIBRARY_DIR  = $(LIBRARY_NAME)
CLIENT_DIR   = client
TESTS_DIR    = tests

SRC_PATH     = ./src
LIBRARY_PATH = $(SRC_PATH)/$(LIBRARY_DIR)
CLIENT_PATH  = $(SRC_PATH)/$(CLIENT_DIR)
TESTS_PATH   = $(SRC_PATH)/$(TESTS_DIR)

BUILD_PATH   = ./build/$(CONFIG)-$(CXX)
OBJECT_PATH  = $(BUILD_PATH)/obj

CUSTOM_INCLUDE_PATH =

#
# Core values
#

APP_VERSION    = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_REVISION)
APP_DATESTAMP  = $(shell date '+"%Y-%m-%d %H:%M"')

LIBRARY_SHARED  = lib$(LIBRARY_NAME).so
LIBRARY_VERSION_SHARED = $(LIBRARY_SHARED).$(VERSION_MAJOR)
LIBRARY_FULL_VERSION_SHARED = $(LIBRARY_SHARED).$(APP_VERSION)
LIBRARY_ARCHIVE = lib$(LIBRARY_NAME).a
PKGCONFIG_FILE  = lib$(LIBRARY_NAME).pc
CLIENT_TARGET   = $(LIBRARY_NAME)
TESTS_TARGET    = $(LIBRARY_NAME)-tests


# OS Specific values

UNAME_S := $(shell uname -s)
LD_EXTRA =
AR_EXTRA = 
ifeq ($(UNAME_S),Linux)
	LD_EXTRA += -Wl,-soname -Wl,$(LIBRARY_VERSION_SHARED)
	AR_EXTRA = crf
endif
ifeq ($(UNAME_S),Darwin)
	LIBRARY_SHARED = lib$(LIBRARY_NAME).dylib
	LIBRARY_VERSION_SHARED = lib$(LIBRARY_NAME).$(VERSION_MAJOR).dylib
	LIBRARY_FULL_VERSION_SHARED = lib$(LIBRARY_NAME).$(APP_VERSION).dylib
	AR_EXTRA = cr
endif


BUILD_SHARED   ?= yes
BUILD_STATIC   ?= yes

CONFIG_FLAGS =
ifeq ($(CONFIG),debug)
	CONFIG_FLAGS = -g -fno-inline -ftemplate-depth-1000
endif
ifeq ($(CONFIG),valgrind)
	CONFIG_FLAGS = -g -O1 -DNO_DEBUG_LOG -DNO_TRACE_LOG
endif
ifeq ($(CONFIG),max)
	CONFIG_FLAGS = -O3 -funroll-loops -ffast-math -finline-functions -fomit-frame-pointer -DNDEBUG
endif
ifneq (,$(findstring $(CONFIG),release loadtest))
	CONFIG_FLAGS = -O3 -funroll-loops -ffast-math -finline-functions -fomit-frame-pointer -DNO_DEBUG_LOG -DNO_TRACE_LOG -DNDEBUG
endif

COMMON_FLAGS = -MMD -std=c++11 -pipe -Wall \
	-DBUILD_ENV=$(CONFIG) \
	-DBUILD_DATESTAMP='$(APP_DATESTAMP)' \
	-DBUILD_LIBRARY_NAME='"$(LIBRARY_NAME)"' \
	-DBUILD_CLIENT_NAME='"$(CLIENT_TARGET)"' \
	-I$(SRC_PATH) $(CUSTOM_INCLUDE_PATH)

ifeq ($(BUILD_SHARED),yes)
COMMON_FLAGS += -fPIC
LIBRARY_TARGETS += $(LIBRARY_SHARED)
endif

ifeq ($(BUILD_STATIC),yes)
LIBRARY_TARGETS += $(LIBRARY_ARCHIVE)
endif

COMMON_LIBS = -lzmq

LIBRARY_LIBS =

CLIENT_LIBS = -L$(BUILD_PATH) \
	-l$(LIBRARY_NAME) \
	-lboost_program_options

TEST_LIBS = -L$(BUILD_PATH) \
	-l$(LIBRARY_NAME) \
	-lboost_unit_test_framework \
	-lpthread

ifeq ($(CONFIG),loadtest)
	CONFIG_FLAGS := $(CONFIG_FLAGS) -DLOADTEST
	TEST_LIBS := $(TEST_LIBS) -lboost_thread -lboost_system
endif

ALL_LIBRARY_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(LIBRARY_PATH) -iname '*.cpp'))

ALL_LIBRARY_INCLUDES := $(shell find $(LIBRARY_PATH) -iname '*.hpp')

ALL_CLIENT_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(CLIENT_PATH) -iname '*.cpp'))

ALL_TEST_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(TESTS_PATH) -iname '*.cpp'))

TEST_SUITES := ${addprefix test-,${sort ${shell find ${TESTS_PATH} -iname *.cpp | xargs grep BOOST_AUTO_TEST_SUITE\( | sed 's/.*BOOST_AUTO_TEST_SUITE( \(.*\) )/\1/' }}}

#
# BUILD Targets - Standardised
#
DEBUG=1
ifeq ($(DEBUG),1)
   BUILD_MODE=debug
endif

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SOURCE = src/

#OBJS = zmq_pty.o
#EXTRA_CMDS += '$( if [ 0 == $(id -u) ]; then chmod a+x bin/* else sudo chmod a+x bin/* ; fi )'

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g -O0
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),profile)
	CFLAGS += -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
	EXTRA_CLEAN += zmq_pty.gcda zmq_pty.gcno $(PROJECT_ROOT)gmon.out
	EXTRA_CMDS = rm -rf zmq_pty.gcda
else
    $(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

GXX = g++

ifeq ("$(DEBUG)","1")
	CFLAGS += -Wall -Wextra -O0 -Wunused
else
	CFLAGS += -Wall -Wextra -O2 -D DOCTEST_CONFIG_DISABLE #-D DEBUG=0
endif

.PHONY: clean uninstall test $(TEST_SUITES)

zmq_pty_client:
	$(GXX) $(CFLAGS) $(LDFLAGS) $(SOURCE)zmq__pty_client.cpp -o bin/$@ $^
	$(EXTRA_CMDS)

zmq_pty_server:
	$(GXX) $(CFLAGS) $(LDFLAGS) $(SOURCE)zmq_pty_server.cpp -o bin/$@ $^
	$(EXTRA_CMDS)

main: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE)
	@echo "use make check to test the build"

examples: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE)
	$(GXX) $(CFLAGS) $(LDFLAGS) examples/simple_client.cpp -o bin/simple_client $^
	$(GXX) $(CFLAGS) $(LDFLAGS) examples/simple_server.cpp -o bin/simple_server $^

all: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(CLIENT_TARGET) zmq_pty_client zmq_pty_server
	@echo "use make check to test the build"

check: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) test

%.o:	$(PROJECT_ROOT)%.cpp
	$(GXX) -c $(CFLAGS) $(GXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

doc:
	@echo "Source code documentation: doc/html/index.html"
	doxygen -u
	doxygen
	$(shell rm Doxyfile.bak)

clean:
	rm -fr zmq_pty $(OBJS) $(EXTRA_CLEAN)
	$(shell touch build/cleanprepare && rm -rf build/*)
	$(shell touch bin/cleanprepare && rm bin/*)
	$(shell touch doc/cleanprepare && rm -rf doc/*)

install:
	sed 's/^Version.*/Version: $(APP_VERSION)/g' $(SRC_PATH)/$(PKGCONFIG_FILE) > $(BUILD_PATH)/$(PKGCONFIG_FILE)
	mkdir -p $(INCLUDEDIR)/$(LIBRARY_DIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(PKGCONFIGDIR)
	install -m 644 $(ALL_LIBRARY_INCLUDES) $(INCLUDEDIR)/$(LIBRARY_DIR)
	install -m 755 $(BUILD_PATH)/$(PKGCONFIG_FILE) $(PKGCONFIGDIR)/$(PKGCONFIG_FILE)
ifeq ($(BUILD_SHARED),yes)
	install -m 755 $(BUILD_PATH)/$(LIBRARY_VERSION_SHARED) $(LIBDIR)/$(LIBRARY_FULL_VERSION_SHARED)
	ln -sf $(LIBRARY_FULL_VERSION_SHARED) $(LIBDIR)/$(LIBRARY_VERSION_SHARED)
	ln -sf $(LIBRARY_FULL_VERSION_SHARED) $(LIBDIR)/$(LIBRARY_SHARED)
endif
ifeq ($(BUILD_STATIC),yes)
	install -m 755 $(BUILD_PATH)/$(LIBRARY_ARCHIVE) $(LIBDIR)/$(LIBRARY_ARCHIVE)
endif
	if [ -f $(BUILD_PATH)/$(CLIENT_TARGET) ]; then install -m 755 $(BUILD_PATH)/$(CLIENT_TARGET) $(BINDIR); fi
	$(LDCONFIG)
	@echo "use make installcheck to test the install"

installcheck: $(TESTS_TARGET)
	$(BUILD_PATH)/$(TESTS_TARGET)

uninstall:
	rm -rf $(INCLUDEDIR)/$(LIBRARY_DIR)
	rm -f $(LIBDIR)/$(LIBRARY_FULL_VERSION_SHARED)
	rm -f $(LIBDIR)/$(LIBRARY_VERSION_SHARED)
	rm -f $(LIBDIR)/$(LIBRARY_SHARED)
	rm -f $(LIBDIR)/$(LIBRARY_ARCHIVE)
	rm -f $(BINDIR)/$(CLIENT_TARGET)

client: $(CLIENT_TARGET)

library: $(LIBRARY_TARGETS)

#
# BUILD Targets
#

$(LIBRARY_SHARED): $(ALL_LIBRARY_OBJECTS)
	$(LD) $(LDFLAGS) -shared -rdynamic $(LD_EXTRA) -o $(BUILD_PATH)/$(LIBRARY_VERSION_SHARED) $^ $(LIBRARY_LIBS) $(COMMON_LIBS)

$(LIBRARY_ARCHIVE): $(ALL_LIBRARY_OBJECTS)
	$(AR) $(AR_EXTRA) $(BUILD_PATH)/$@ $^

$(CLIENT_TARGET): $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(ALL_CLIENT_OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_PATH)/$@ $(ALL_CLIENT_OBJECTS) $(CLIENT_LIBS) $(COMMON_LIBS)

$(TESTS_TARGET): $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(ALL_TEST_OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_PATH)/$@ $(ALL_TEST_OBJECTS) $(TEST_LIBS) $(COMMON_LIBS)

$(TEST_SUITES): $(TESTS_TARGET)
	LD_LIBRARY_PATH=$(BUILD_PATH):$(LD_LIBRARY_PATH) $(BUILD_PATH)/$(TESTS_TARGET) --log_level=message --run_test=$(patsubst test-%,%,$@)

test: $(TESTS_TARGET)
	@echo "running all test targets ($(TEST_SUITES))"
	LD_LIBRARY_PATH=$(BUILD_PATH):$(LD_LIBRARY_PATH) $(BUILD_PATH)/$(TESTS_TARGET)

#
# Dependencies
# We don't care if they don't exist as the object won't have been built
#

-include $(patsubst %.o,%.d,$(ALL_LIBRARY_OBJECTS) $(ALL_TEST_OBJECTS))

#
# Object file generation
#

$(OBJECT_PATH)/%.o: $(SRC_PATH)/%.cpp
	-mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMMON_FLAGS) $(CONFIG_FLAGS) -c -o $@ $<

