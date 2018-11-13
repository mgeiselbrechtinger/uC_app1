
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./menu -I./libglcd -I./libwiimote -I./libsdcard -I./libmp3 -I./music -I./adc -I./rand
LDFLAGS  := -L. -L./libglcd -L./libwiimote -L./libmp3 -L./libsdcard

OBJS	 := main.o menu/menu.o libwiimote/hal_wt41_fc_uart.o libsdcard/spi.o music/music.o adc/adc.o rand/rand.o
LIBS	 := -lglcd -lwiimote -lmp3 -lsdcard
LIBDEPS	 := libglcd/libglcd.a libwiimote/libwiimote.a libmp3/libmp3.a libsdcard/libsdcard.a

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
	rm -f main.elf main.o adc/adc.o libsdcard/spi.o libwiimote/hal_wt41_fc_uart.o menu/menu.o music/music.o rand/rand.o
