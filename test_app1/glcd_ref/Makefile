
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./hal/ -I./glcd_user/ -I./font/
LDFLAGS  := -L. -L./libglcd/

OBJS	 := main.o font/Standard5x7.o
LIBS	 := -lglcd
LIBDEPS	 := libglcd/libglcd.a

.PHONY: all
all: main.elf

assembly: main.s hal/hal_glcd.s

%.s: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) $(CPPFLAGS) -S -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<

main.elf: $(OBJS) $(LIBDEPS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: install
install: main.elf
	$(AVRPROG2) --flash w:$<

.PHONY: clean
clean:
	rm -f main.elf $(OBJS)
