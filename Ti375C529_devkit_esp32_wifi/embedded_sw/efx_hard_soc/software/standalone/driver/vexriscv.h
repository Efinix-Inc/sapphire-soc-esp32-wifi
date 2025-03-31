///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "riscv.h"

//Invalidate the whole data cache
#define data_cache_invalidate_all() asm(".word(0x500F)");

//Invalidate all the data cache ways lines which could store the given address
#define data_cache_invalidate_address(address)     \
({                                             \
    asm volatile(                              \
     ".word ((0x500F) | (regnum_%0 << 15));"   \
     :                                         \
     : "r" (address)                               \
    );                                         \
})

//Invalidate the whole instruction cache
#define instruction_cache_invalidate() asm("fence.i");

//Write buffer flush
#define soc_write_buffer_flush()     \
({                                   \
    csr_write(0x810, 1);             \
    while(csr_read(0x810) & 1);      \
})

