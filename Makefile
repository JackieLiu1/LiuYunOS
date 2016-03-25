OBJS = main.o console.o string.o kalloc.o proc.o trapasm.o

CC = gcc
LD = ld
CLANG = clang
OBJDUMP = objdump
OBJCOPY = objcopy

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)

LiuYun.img : bootblock kernel
	dd if=/dev/zero of=LiuYun.img count=1000
	dd if=bootblock of=LiuYun.img conv=notrunc
	dd if=kernel of=LiuYun.img seek=1 conv=notrunc

bootblock : bootasm.S bootmain.c
	$(CC) -O $(CFLAGS) -I. -nostdinc -c bootmain.c
	$(CC) $(CFLAGS) -I. -nostdinc -c bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7c00 -o bootblock.o bootasm.o bootmain.o
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	./sign.pl bootblock

kernel : $(OBJS)
	$(LD) $(LDFLAGS) -Ttext 0x100000 -e main -o kernel $(OBJS)

%.o : %.c
	$(CLANG) $(CFLAGS) -I. -c -o $@ $<

clean:
	rm -rf *.o bootblock *.d
	rm -rf kernel LiuYun.img
