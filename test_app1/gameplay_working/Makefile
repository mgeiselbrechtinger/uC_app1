
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -funsigned-char -funsigned-bitfields -fshort-enums -fpack-struct #-O2
CPPFLAGS := -I. -I./menu -I./libwiimote -I./libsdcard -I./libmp3 -I./music -I./adc -I./rand -I./glcd/hal -I./glcd/glcd_user -I.glcd/font -I./libglcd
LDFLAGS  := -L. -L./libwiimote -L./libmp3 -L./libsdcard -L./libglcd 

OBJS	 := main.o menu/menu.o libwiimote/hal_wt41_fc_uart.o libsdcard/spi.o music/music.o adc/adc.o rand/rand.o glcd/hal/hal_glcd.o glcd/glcd_user/glcd.o glcd/font/Standard5x7.o
LIBS	 := -lwiimote -lmp3 -lsdcard #-lglcd
LIBDEPS	 := libwiimote/libwiimote.a libmp3/libmp3.a libsdcard/libsdcard.a #libglcd/libglcd.a 

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
