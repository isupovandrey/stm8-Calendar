TARGET=Calendar

CFLAGS	= -mstm8 
LDFLAGS	= --out-fmt-ihx
OBJ =  $(patsubst  %.c, ./debug/%.rel,$(wildcard *.c))



all: CheckDebug $(TARGET).ihx

debug/%.rel:	%.c $(wildcard *.h)
	sdcc $(CFLAGS) -c $< -o ./debug/

$(TARGET).ihx: $(OBJ)
	sdcc $(CFLAGS) $(LDFLAGS) -o ./debug/$(TARGET).ihx  $(OBJ)

size:
	@grep CODE -w debug/$(TARGET).map

clean:
	rm ./debug/*

CheckDebug:
	@mkdir -p debug

flash:
	stm8flash -c stlinkv2 -p stm8s003f3 -w debug/$(TARGET).ihx