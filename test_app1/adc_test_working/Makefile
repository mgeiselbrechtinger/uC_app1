
CC := avr-gcc
LD := $(CC)
AVRPROG2 := avrprog2

CFLAGS   := -mmcu=atmega1280
CCFLAGS  := -std=gnu99 -Wall -Wno-main -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS := -I. -I./adc
LDFLAGS  := 

OBJS	 := main.o adc/adc.o
LIBS	 := 
LIBDEPS	 :=

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
	rm -f main.elf main.o adc/adc.o
