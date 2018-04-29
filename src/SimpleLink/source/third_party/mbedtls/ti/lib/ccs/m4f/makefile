CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

CFLAGS = $(INCS) "-I$(CCS_ARMCOMPILER)/include" --silicon_version=7M4 \
        --code_state=16 --little_endian --display_error_number \
        --diag_warning=255 --diag_wrap=off --gen_func_subsections=on \
        --float_support=FPv4SPD16 --symdebug:dwarf

ARFLAGS = -r

vpath %.c ../../../../library ../../../port

%.obj: %.c
	@ echo "#"
	@ echo "# Building $@"
	$(CC) $(POSIX_INCS_CCS) $(CFLAGS) $< -fe $@

all: mbedtls.a

clean:
	@ rm -f *.a *.obj

mbedtls.a: $(OBJS)
	@ echo "#"
	@ echo "# Building $@"
	$(AR) $(ARFLAGS) $@ $^
