#Usage: make CROSS_PLATFORM=XXX
#其中XXX代表不同的硬件平台，不带CROSS_PLATFORM参数默认使用GK7201V200

CROSS_PLATFORM ?= GK7201V200

CC  = gcc
CPP = g++

PROJECT_ROOT := ..
TARGET := GB28181Demo
SIP_OUTPUT := $(PROJECT_ROOT)/SIP/Vendor/resiprocate/cmake/output

CSRCS += 
CPPSRCS += test.cpp

OBJS := $(patsubst %.c, %.o, $(CSRCS))
OBJS += $(patsubst %.cpp, %.o, $(CPPSRCS))

DEPS := $(OBJS:.o=.d)

INCLUDES := -I$(PROJECT_ROOT)
INCLUDES += -I$(PROJECT_ROOT)/Agent
INCLUDES += -I$(PROJECT_ROOT)/MANSCDP
INCLUDES += -I$(PROJECT_ROOT)/Process
INCLUDES += -I$(PROJECT_ROOT)/UserInterface
INCLUDES += -I$(PROJECT_ROOT)/tinyxml2
INCLUDES += -I$(PROJECT_ROOT)/SIP/Adapter

CFLAGS := $(INCLUDES) -g -MMD

CPPFLAGS := $(CFLAGS)

LDFLAGS := -L$(PROJECT_ROOT)/Build/
LDFLAGS += -lGB28181

#不同的硬件平台
ifeq ($(CROSS_PLATFORM), GK7201V200)
	CC  := /opt/goke/arm-gcc7.3-linux-uclibceabi/bin/arm-gcc7.3-linux-uclibceabi-gcc
	CPP := /opt/goke/arm-gcc7.3-linux-uclibceabi/bin/arm-gcc7.3-linux-uclibceabi-g++
else ifeq ($(CROSS_PLATFORM), X86GCC7.5)
	CC  := /usr/bin/gcc
	CPP := /usr/bin/g++
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
	CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer
	
	CFLAGS += -I$(SIP_OUTPUT)/include
	CPPFLAGS += -I$(SIP_OUTPUT)/sinclude
	LDFLAGS += -fsanitize=address
	LDFLAGS += -L$(SIP_OUTPUT)/lib -ldum -lresip -lrutil -lresipares -pthread
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
