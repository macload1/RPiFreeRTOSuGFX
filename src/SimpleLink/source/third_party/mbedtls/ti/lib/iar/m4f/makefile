CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

CFLAGS = $(INCS) "-I$(IAR_ARMCOMPILER)/inc" --silent --aeabi --cpu=Cortex-M4F \
        --diag_suppress=Pa050,Go005 --endian=little -e --fpu=VFPv4_sp --thumb

vpath %.c ../../../../library ../../../port

%.obj: %.c
	@ echo "#"
	@ echo "# Building $@"
	$(CC) $(POSIX_INCS_IAR) $(CFLAGS) $< -o $@

all: mbedtls.a

clean:
	@ rm -f *.a *.obj

mbedtls.a: $(OBJS)
	@ echo "#"
	@ echo "# Building $@"
	@ rm -f $@
	$(AR) $@ $^
