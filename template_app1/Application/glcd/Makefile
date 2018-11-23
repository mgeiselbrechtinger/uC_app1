
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./hal/ -I./glcd_user/ -I./font/
LDFLAGS  := -L.

OBJS	 := main.o hal/hal_glcd.o glcd_user/glcd.o font/Standard5x7.o
LIBS	 := 
LIBDEPS	 := 

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
