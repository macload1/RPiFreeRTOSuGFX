# build environment
PREFIX ?= /home/macload1/RPi/BareMetal/gcc-arm-none-eabi-5_4-2016q3
ARCH ?= $(PREFIX)/bin/arm-none-eabi

CC = ${ARCH}-gcc
CPP = ${ARCH}-g++
AS = ${ARCH}-as
LD = ${ARCH}-gcc
AR = ${ARCH}-ar
OBJCOPY = ${ARCH}-objcopy


PLATFORM = raspi
LINKER_SCRIPT = raspberrypi.ld

CFLAGS = -march=armv6z -g -Wall -Wextra -D__HEAP_SIZE=1024 #-std=c99 # -DSL_FULL
ASFLAGS = -g 

CFLAGS_FOR_TARGET = #-mcpu=arm1176jzf-s
ASFLAGS_FOR_TARGET = #-mcpu=arm1176jzf-s
LDFLAGS = -nostartfiles #--specs=rdimon.specs #--error-unresolved-symbols

GFXLIB := src/uGFX
include $(GFXLIB)/gfx.mk
include $(GFXLIB)/drivers/gdisp/framebuffer/driver.mk

MODULES := FreeRTOS/Source/portable/GCC/RaspberryPi
MODULES += FreeRTOS/Source/portable/MemMang
MODULES += FreeRTOS/Source
MODULES += Demo/Drivers
MODULES += Demo/Graphics
MODULES += Demo/Tasks
MODULES += Demo

SRC_DIR := $(addprefix src/,$(MODULES))
INC_DIR := $(addsuffix /include,$(SRC_DIR))
BUILD_DIR := $(addsuffix /build,$(SRC_DIR))

INCLUDEDIRS := src/FreeRTOS/Source/portable/GCC/RaspberryPi
INCLUDEDIRS += src/FreeRTOS/Source/include
INCLUDEDIRS += src/Demo/Drivers
INCLUDEDIRS += src/Demo/Graphics
INCLUDEDIRS += src/Demo/Tasks
INCLUDEDIRS += src/Demo
INCLUDEDIRS += $(GFXINC)

INCLUDES := $(addprefix -I,$(INCLUDEDIRS))

ASRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.s))
AOBJ     := $(ASRC:.s=.o)
CSRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
CSRC += $(GFXSRC)
COBJ     := $(CSRC:.c=.o)

vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.s $(SRC_DIR)

%.o: %.c
	$(CC) $(CFLAGS_FOR_TARGET) $(INCLUDES) $(CFLAGS) -c -o $*.o $<

%.o: %.s
	$(AS) $(ASFLAGS_FOR_TARGET) $(INCLUDES) $(ASFLAGS) -o $*.o $<

OBJ = $(AOBJ) $(COBJ)

SDCard/kernel.img: bin/kernel.elf
	${OBJCOPY} -O binary $< $@

#bin/kernel.elf: LDFLAGS += -L "/home/macload1/RPi/BareMetal/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/4.8.3" -lgcc 
bin/kernel.elf: LDFLAGS += -L "/home/macload1/RPi/BareMetal/gcc-arm-none-eabi-5_4-2016q3/arm-none-eabi/lib" -lc
bin/kernel.elf: LDFLAGS += -L "/home/macload1/RPi/BareMetal/gcc-arm-none-eabi-5_4-2016q3/arm-none-eabi/lib" -lrdimon
#bin/kernel.elf: LDFLAGS += -L "/home/macload1/RPi/BareMetal/gcc-arm-none-eabi-4_8-2014q1/arm-none-eabi/lib" -lstdc++
#bin/kernel.elf: LDFLAGS += -L "/home/macload1/RPi/BareMetal/gcc-arm-none-eabi-4_8-2014q1/arm-none-eabi/lib" -lm
bin/kernel.elf: $(OBJ)
	${LD} $(OBJ) -o $@ -T $(LINKER_SCRIPT) ${LDFLAGS} -Wl,-Map=bin/kernel.map

clean:
	rm -f bin/*.elf bin/*.img bin/*.map $(OBJ)



