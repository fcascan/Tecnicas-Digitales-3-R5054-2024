/*
    Definiciones necesarias: formato de salida, arquitectura y punto de entrada
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(file)

/*
    Definiciones de simbolos necesarios
*/
_PUBLIC_RAM_INIT    =   0x70010000;  
_PUBLIC_STACK_INIT  =   0X70020000;

/*
    Definiciones del mapa de memoria
*/
MEMORY{
    public_ram : org = _PUBLIC_RAM_INIT,    len = 0x1000
    public_stack : org = _PUBLIC_STACK_INIT, len = 0x1000
}

/*
    Definicion de las secciones
    . location pointer: simboliza la dir en la que estoy trabajando ahora
    > public_ram donde lo pongo
*/
SECTIONS{
    . = _PUBLIC_RAM_INIT; /*direccion de inicio*/
    .text : { /* Poneme todo el ensamblado de la seccion text contiguo*/
        *(.text*)
        } > public_ram
    /*salida    entrada*/
    .data : {*(.data*)} > public_ram /* despues con la seccion de data*/
    .bss : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > public_ram
}
