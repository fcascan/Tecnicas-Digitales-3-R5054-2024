# KERNEL_SOURCE = /lib/modules/3.8.13-bone84/build
KERNEL_SOURCE = /usr/src/linux-headers-4.14.71-ti-r80
# KERNEL_SOURCE = /lib/modules/$(shell uname -r)/build
# KERNEL_SOURCE = /usr/src/linux-headers-$(shell uname -r)/build
MOD_NAME = td3_i2c_fcc

# Paths del driver
PWD_DRIVER = $(shell pwd)/platform-driver
OBJ_DRIVER = $(PWD_DRIVER)/obj
INC_DRIVER = $(PWD_DRIVER)/inc
SRC_DRIVER = $(PWD_DRIVER)/src

# Paths del test
PWD_TEST = $(shell pwd)/driver-test
OBJ_TEST = $(PWD_TEST)/obj
SRC_TEST = $(PWD_TEST)/src

# Convierte un módulo objeto en LKM (Indica que es un módulo del kernel)
obj-m := $(MOD_NAME).o

# Compila el módulo haciendo una llamada al Makefile del kernel
all: directories build clean_temp_files insmod

directories:
	@echo ">> Creando directorios..."
	mkdir -p $(OBJ_DRIVER) $(OBJ_TEST)

# Compilación del módulo en el directorio de trabajo y movimiento de archivos
build:
	@echo ">> Compilando el módulo..."
	$(MAKE) -C $(KERNEL_SOURCE) M=$(shell pwd) EXTRA_CFLAGS="-I$(INC_DRIVER) -Wall" W=1 modules
	mv $(MOD_NAME).ko $(OBJ_DRIVER)/
	@echo ">> Compilación completada, archivo .ko movido a $(OBJ_DRIVER)"

# Limpia todos los archivos temporales generados durante la compilación
clean_temp_files:
	@echo ">> Limpiando archivos temporales..."
	rm -f $(PWD)/*.mod.o $(PWD_DRIVER)/*.o $(PWD_DRIVER)/*.mod.c
	rm -f $(PWD)/modules.order $(PWD_DRIVER)/Module.symvers
	find $(PWD) -name '*.o' -type f -delete
	find $(PWD) -name '*.cmd' -type f -delete
	find $(PWD) -name '*.mod' -type f -delete
	find $(PWD) -name '*.order' -type f -delete
	rm -f $(PWD)/Module.symvers $(PWD)/$(MOD_NAME).mod.c
	@echo ">> Archivos temporales limpiados."

# Limpia todos los archivos generados
softclean:
	@echo ">> Limpiando archivos generados..."
	$(MAKE) -C $(KERNEL_SOURCE) M=$(shell pwd) clean || true
	rm -rf $(OBJ_DRIVER)
	@echo ">> Archivos generados limpiados."

# Desinstala el módulo y limpia los archivos generados
clean: rmmod softclean

# Desinstala el módulo, lo compila y lo instala
rebuild: rmmod all

# Revisa si el módulo está instalado
cat:
	cat /proc/modules | grep $(MOD_NAME)

# Instala el módulo
insmod:
	@echo ">> Verificando si el módulo ya está cargado..."
	@if lsmod | grep $(MOD_NAME) &> /dev/null ; then \
		echo "El módulo $(MOD_NAME) ya está cargado. Desinstalando..."; \
		sudo rmmod -f  $(MOD_NAME) || true; \
	else \
		echo "El módulo $(MOD_NAME) no está cargado."; \
	fi
	@echo ">> Instalando el módulo..."
	sudo insmod $(OBJ_DRIVER)/$(MOD_NAME).ko
	@echo ">> Módulo instalado."

# Desinstala el módulo
rmmod:
	@echo ">> Desinstalando el módulo..."
	sudo rmmod -f $(MOD_NAME)
	@echo ">> Módulo desinstalado."

# Muestra los mensajes (dmesg)
dmesg:
	dmesg | grep $(MOD_NAME)

# Muestra los logs del kernel
tail:
	tail -f /var/log/syslog

info:
	modinfo $(OBJ_DRIVER)/$(MOD_NAME).ko

dev:
	ls -l /dev/ | grep $(MOD_NAME)

# Realiza un test simple del driver
test: compile_test
	$(OBJ_TEST)/driver_test

compile_test:
	@echo ">> Compilando test..."
	gcc -o $(OBJ_TEST)/driver_test $(SRC_TEST)/driver_test.c
	@echo ">> Test compilado."
