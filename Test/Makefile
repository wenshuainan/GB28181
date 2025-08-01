#Usage: make CROSS_PLATFORM=XXX
#其中XXX代表不同的硬件平台，不带CROSS_PLATFORM参数默认使用X86GCC7.5

CROSS_PLATFORM ?= X86GCC7.5

CC  = gcc
CPP = g++

LIB_ROOT := ../Build/output
TARGET := GB28181Test

CSRCS += 
CPPSRCS += test.cpp

OBJS := $(patsubst %.c, %.o, $(CSRCS))
OBJS += $(patsubst %.cpp, %.o, $(CPPSRCS))

DEPS := $(OBJS:.o=.d)

INCLUDES := -I..
INCLUDES += -I$(LIB_ROOT)/include

CFLAGS := $(INCLUDES) -g -MMD

CPPFLAGS := $(CFLAGS)

LDFLAGS := -L$(LIB_ROOT)/lib
LDFLAGS += -Wl,--start-group -lGB28181 -lrtp -lmpeg2 -lsipadapter -ldum -lresip -lrutil -lresipares -ltinyxml2 -pthread -Wl,--end-group
LDFLAGS += -Wl,--gc-sections

#不同的硬件平台
ifeq ($(CROSS_PLATFORM), X86GCC7.5)
	CC  := /usr/bin/gcc
	CPP := /usr/bin/g++
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
	CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
else ifeq ($(CROSS_PLATFORM), GK7201V200)
	CC  := /opt/goke/arm-gcc7.3-linux-uclibceabi/bin/arm-gcc7.3-linux-uclibceabi-gcc
	CPP := /opt/goke/arm-gcc7.3-linux-uclibceabi/bin/arm-gcc7.3-linux-uclibceabi-g++
endif

.PHONY: all clean

all: $(OBJS)
	$(CPP) -o $(TARGET) $^ $(LDFLAGS)
	
-include $(DEPS)

%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)
	
%.o : %.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -rf $(OBJS) $(TARGET) $(DEPS)
