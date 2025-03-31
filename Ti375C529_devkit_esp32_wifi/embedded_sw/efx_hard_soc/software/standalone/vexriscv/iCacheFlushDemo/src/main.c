////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include <string.h>

const char* funcA(){
    return "funcA\r\n";
}

const char* funcB(){
    return "funcB\r\n";
}

uint32_t array[64]  __attribute__ ((aligned (64)));

void main() {
    bsp_init();

    bsp_printf("***Starting Flush Instruction Cache Demo*** \r\n");
    
    const char* (*funcPtr)() = (const char* (*)())array;
    bsp_printf("Memcpy funcA into array .. \r\n");
    memcpy(array, funcA, 64*4);
    bsp_printf("Flush the instruction cache once to avoid preloaded data .. \r\n");
    asm("fence.i"); 
    bsp_printf("Expected 'funcA', Obtained : ");
    bsp_printf(funcPtr()); 

    bsp_printf("Memcpy funcB into array .. \r\n");
    memcpy(array, funcB, 64*4);
    bsp_printf("Expected 'funcA', Obtained : ");
    bsp_printf(funcPtr());
    bsp_printf("Still get the FuncA as there is no cache flush .. \r\n");

    bsp_printf("Flush the instruction cache now .. \r\n");
    asm("fence.i"); 
    bsp_printf("Expected 'funcB', Obtained : ");
    bsp_printf(funcPtr());
    
    bsp_printf("***Successfully Ran Demo*** \r\n");
}
