void *td3_memcpy(void *destino ,const void *origen, unsigned int num_bytes );

__attribute__((section(".text_memcopy")))void *td3_memcpy(void *destino ,const void *origen, unsigned int num_bytes )
{
    int* destino_cpy    = (int*) destino;
    int* origen_cpy     = (int*) origen;

    if(num_bytes > 0) {
        for(int i = 0; i < num_bytes; i++) {
           *(destino_cpy+i) = *(origen_cpy+i);
        }
        return (void *) destino_cpy;
    } else {
        return 0;
    }
}
