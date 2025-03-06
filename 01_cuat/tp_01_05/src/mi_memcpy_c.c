void *td3_memcpy(void *destino, const void *origen, unsigned int num_bytes );
__attribute__((section("._memcopy")))void *td3_memcpy(void *destino, const void *origen, unsigned int num_bytes)
{
    unsigned char* destino_cpy = (unsigned char*) destino;
    unsigned char* origen_cpy =  (unsigned char*) origen;
    
    for(int i=0; i<num_bytes; i++)
        destino_cpy[i] = origen_cpy[i];
        
}
