#ifndef __COMMON_LIB_H
#define __COMMON_LIB_H
    //Pongo la funcion reserved_bits aca para que no tire warning de redifinicion
      #define reserved_bits(x,y,z) uint8_t reserved##x[ z - y + 1 ];
#endif // __COMMON_LIB_H
