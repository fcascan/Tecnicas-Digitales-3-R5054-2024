CHAIN=arm-none-eabi
CFLAGS=-std=gnu99 -Wall -mcpu=cortex-a8
LDEEXTRAS= -lc -L /usr/lib/arm-none-eabi/newlib/

FILENAME_OUT= ej_timer

OBJ=obj/
BIN=bin/
INC=inc/
SRC=src/
LST=lst/

SRC_S = $(wildcard $(SRC)*.s)
OBJ_S = $(patsubst $(SRC)%.s,$(OBJ)%.o,$(SRC_S))

SRC_C = $(wildcard $(SRC)*.c)
OBJ_C = $(patsubst $(SRC)%.c,$(OBJ)%.o,$(SRC_C))

quick: clean all run_ddd

all_wclean: clean all

run_ddd:
	qemu-system-arm -M realview-pb-a8 -m 32M -no-reboot -nographic -monitor telnet:127.0.0.1:1234,server,nowait -kernel $(BIN)$(FILENAME_OUT).bin -S -gdb tcp::2159

run:
	qemu-system-arm -M realview-pb-a8 -m 32M -no-reboot -nographic -monitor telnet:127.0.0.1:1234,server,nowait -kernel $(BIN)$(FILENAME_OUT).bin

symb:
	$(CHAIN)-objcopy --only-keep-debug $(OBJ)$(FILENAME_OUT).elf $(BIN)$(FILENAME_OUT).sym

all: $(BIN)$(FILENAME_OUT).bin $(OBJ)$(FILENAME_OUT).elf

$(BIN)$(FILENAME_OUT).bin: $(OBJ)$(FILENAME_OUT).elf
	$(CHAIN)-objcopy -O binary $< $@


$(OBJ)$(FILENAME_OUT).elf: $(OBJ_S) $(OBJ_C)
	@echo "Linkeando..."
	mkdir -p obj
	mkdir -p lst
	#$(CHAIN)-ld $(OBJ)*.o -o $(OBJ)$(FILENAME_OUT).elf -Map $(LST)$(FILENAME_OUT).map
	$(CHAIN)-ld -g -T td3_memmap.ld $(LDEEXTRAS) $(OBJ)*.o -o $(OBJ)$(FILENAME_OUT).elf -Map $(LST)$(FILENAME_OUT).map
	@echo "Linkeo finalizado!"
	@echo ""
	@echo "Generando archivos de información: mapa de memoria y sìmbolos"
	readelf -a $(OBJ)$(FILENAME_OUT).elf > $(LST)$(FILENAME_OUT).txt
	$(CHAIN)-objdump -D $(OBJ)$(FILENAME_OUT).elf > $(LST)$(FILENAME_OUT).lst

$(OBJ)%.o: $(SRC)%.s
	@echo ""
	mkdir -p bin
	mkdir -p obj
	mkdir -p lst
	@echo "Ensamblando..."
	$(CHAIN)-as -g $< -o $@ -a > $(patsubst $(OBJ)%.o,$(LST)%.lst,$@)

$(OBJ)%.o: $(SRC)%.c
	@echo ""
	mkdir -p bin
	mkdir -p obj
	mkdir -p lst
	@echo "Compilando..."
	$(CHAIN)-gcc -O3 $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)*.o
	rm -rf $(OBJ)*.elf
	rm -rf $(BIN)*.bin
	rm -rf $(LST)*.lst
	rm -rf $(LST)*.txt
	rm -rf $(LST)*.map