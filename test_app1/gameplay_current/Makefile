
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./menu -I./libwiimote -I./libsdcard -I./libmp3 -I./music -I./adc -I./rand -I./font/ -I./hal/ -I./glcd_user/
LDFLAGS  := -L. -L./libwiimote -L./libmp3 -L./libsdcard

OBJS	 := main.o menu/menu.o libwiimote/hal_wt41_fc_uart.o libsdcard/spi.o music/music.o adc/adc.o rand/rand.o hal/hal_glcd.o glcd_user/glcd.o font/Standard5x7.o
LIBS	 := -lwiimote -lmp3 -lsdcard
LIBDEPS	 := libwiimote/libwiimote.a libmp3/libmp3.a libsdcard/libsdcard.a

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
	rm -f main.elf $(OBJS)
