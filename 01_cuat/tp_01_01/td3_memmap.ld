/*
    Definiciones necesarias: formato de salida, arquitectura y punto de entrada
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)

/*
    Definiciones de simbolos necesarios
*/
_PUBLIC_ROM_INIT    =   0x70010000;  
_PUBLIC_STACK_INIT  =   0X70020000;
_PUBLIC_RAM_INIT    =   0X70030000;

/*
    Definiciones del mapa de memoria
*/
MEMORY{
    public_rom         : org = _PUBLIC_ROM_INIT,    len = 0x1000
    public_ram         : org = _PUBLIC_RAM_INIT,    len = 0x1000
    public_stack       : org = _PUBLIC_STACK_INIT,  len = 0x1000
}

/*
    Definicion de las secciones
    . location pointer: simboliza la dir en la que estoy trabajando ahora
    >public_ram donde lo pongo
*/
/* Location pointer .referecian a una direccion */
    /* direccion de inicio */ 
    /* Colocar todo lo ensamblado de la seccion .text contiguo */
SECTIONS{
    
    . = _PUBLIC_ROM_INIT; 
    .text : { 
        *(._reset_code)
        *(._start_code)
        *(.text*)
    } > public_rom
    _text_size = SIZEOF(.text);
  
    __kernel_init__ = LOADADDR(.text) + SIZEOF(.text);

/*
    La seccion kernel se ensambla a continuacion de la seccion text y va a parar a la rom
*/
    .kernel : AT (__kernel_init__){
        *(.kernel)
    } > public_rom

    _kernelSize_ =  SIZEOF(.kernel);
    
    .data : {*(.data*)} > public_ram /* despues con la seccion de data*/
    .bss : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > public_ram
}
