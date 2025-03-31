////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "vexriscv.h"

#define LOC_EXT_MEM ((volatile uint32_t*)0x00100000)
#define NUM 8

void main() 
{
    bsp_init();
    bsp_printf("***Starting Invalidate Data Cache Demo*** \r\n");

    bsp_printf("Invalidate 3 cache lines .. \r\n");
    for(int j=0; j<NUM; j++){
        data_cache_invalidate_address(LOC_EXT_MEM+j);
    }
    bsp_printf("Invalidate all cache line .. \r\n");
    data_cache_invalidate_all();
    
    bsp_printf("***Succesfully Ran Demo*** \r\n");
}


