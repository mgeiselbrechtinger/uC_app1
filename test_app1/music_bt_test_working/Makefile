
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -fpack-struct -fshort-enums -Wno-main -O2 -funsigned-char -funsigned-bitfields 
CPPFLAGS := -I. -I./libsdcard -I./libmp3 -I./music -I./adc -I./rand
LDFLAGS  := -L. -L./libmp3 -L./libsdcard

OBJS	 := main.o libsdcard/spi.o music/music.o adc/adc.o rand/rand.o
LIBS	 := -lmp3 -lsdcard
LIBDEPS	 := libmp3/libmp3.a libsdcard/libsdcard.a

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
	rm -f main.elf main.o libsdcard/spi.o
