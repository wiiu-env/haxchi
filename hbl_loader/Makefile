PATH := $(DEVKITPPC)/bin:$(PATH)
PREFIX ?= powerpc-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc
CFLAGS = -std=gnu99 -O3 -nostdinc -fno-builtin
ASFLAGS = -mregnames -x assembler-with-cpp
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
GCC_VER := $(shell $(DEVKITPPC)/bin/powerpc-eabi-gcc -dumpversion)
LDFLAGS=-Ttext 1800000 -L$(DEVKITPPC)/lib/gcc/powerpc-eabi/$(GCC_VER) -lgcc
OBJDUMP ?= $(PREFIX)objdump
project	:=	.
root	:=	$(CURDIR)
build	:=	 $(root)/bin

FIRMWARE = 550

all: clean setup main

setup:
	mkdir -p $(root)/bin/

main:
	$(CC) $(CFLAGS) -DVER=$(FIRMWARE) -c $(project)/launcher.c
	$(AS) $(ASFLAGS) -DVER=$(FIRMWARE) -c $(project)/crt0.S
	cp -r $(root)/*.o $(build)
	rm $(root)/*.o
	$(LD) -o hbl_loader.elf $(build)/crt0.o `find $(build) -name "*.o" ! -name "crt0.o"` $(LDFLAGS) -Map hbl_loader.map
	$(OBJCOPY) hbl_loader.elf -S -O binary ../hbl_loader.bin

clean:
	rm -rf $(build)
	rm -rf hbl_loader.elf hbl_loader.map