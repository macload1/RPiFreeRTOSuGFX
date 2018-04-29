CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
AR = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc-ar"

CFLAGS= $(INCS) -mcpu=cortex-m4 -march=armv7e-m -mthumb \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections \
        -fdata-sections -g -gstrict-dwarf -Wall -c -D_POSIX_SOURCE

ARFLAGS = -r

vpath %.c ../../../../library ../../../port

%.obj: %.c
	@ echo "#"
	@ echo "# Building $@"
	$(CC) $(POSIX_INCS_GCC) $(CFLAGS) $< -o $@

all: mbedtls.a

clean:
	@ rm -f *.a *.obj

mbedtls.a: $(OBJS)
	@ echo "#"
	@ echo "# Building $@"
	$(AR) $(ARFLAGS) $@ $^
