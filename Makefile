ARCH = arm-none-eabi
CC = ${ARCH}-gcc
CPP = ${ARCH}-g++
AS = ${ARCH}-as
LD = ${ARCH}-ld
AR = ${ARCH}-ar
OBJCOPY = ${ARCH}-objcopy

PLATFORM = raspi

# Release Version -> Optimize 
#CFLAGS = -O3 -std=gnu99 -Werror -D__$(PLATFORM)__ -DRASPBERRY_PI
#ASFLAGS =

CFLAGS = -O0 -g -std=gnu99 -Werror -D__$(PLATFORM)__ -DRASPBERRY_PI -fno-builtin -lgcc #-fno-exceptions -fnon-call-exceptions
ASFLAGS = -g

CFLAGS_FOR_TARGET = -mcpu=arm1176jzf-s
ASFLAGS_FOR_TARGET = -mcpu=arm1176jzf-s
LDFLAGS = -static -nostdlib  --error-unresolved-symbols 


MODULES := raspi kickstart/lib.exec kickstart/dev.timer kickstart/lib.usb/driver kickstart/lib.usb kickstart/dev.sdhci kickstart/dev.sdhci/driver
SRC_DIR := $(addprefix src/,$(MODULES))
INC_DIR := $(addsuffix /include,$(SRC_DIR))
BUILD_DIR := $(addsuffix /build,$(SRC_DIR))

ASRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.s))
AOBJ     := $(ASRC:.s=.o)
CSRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
COBJ     := $(CSRC:.c=.o)

INCLUDES := -Isrc $(addprefix -I,$(SRC_DIR) $(INC_DIR))

vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.s $(SRC_DIR)

%.o: %.c
	$(CC) $(CFLAGS_FOR_TARGET) $(INCLUDES) $(CFLAGS) -c -o $*.o $<

%.o: %.s
	$(AS) $(ASFLAGS_FOR_TARGET) $(INCLUDES) $(ASFLAGS) -o $*.o $<

OBJ = $(AOBJ) $(COBJ)

bin/kernel.img: bin/kernel.elf
	${OBJCOPY} -O binary $< $@

bin/kernel.elf: raspi.ld $(OBJ) 
	${LD} ${LDFLAGS}  $(OBJ) -Map bin/kernel.map -o $@ -T raspi.ld

clean:
	rm -f bin/*.elf bin/*.img bin/*.map $(OBJ)

