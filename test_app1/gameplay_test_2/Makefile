
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./menu -I./libglcd -I./libwiimote
LDFLAGS  := -L. -L./libglcd -L./libwiimote

OBJS	 := main.o menu/menu.o libwiimote/hal_wt41_fc_uart.o
LIBS	 := -lglcd -lwiimote
LIBDEPS	 := libglcd/libglcd.a libwiimote/libwiimote.a

.PHONY: all
all: main.elf

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<

main.elf: $(OBJS) $(LIBDEPS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: install
install: main.elf
	$(AVRPROG2) --flash w:$<

.PHONY: clean
clean:
	rm -f main.elf main.o gameplay/gameplay.o
