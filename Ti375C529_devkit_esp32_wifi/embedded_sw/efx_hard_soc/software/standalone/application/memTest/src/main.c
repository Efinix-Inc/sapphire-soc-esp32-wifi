///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "userDef.h"

void main() {

    bsp_printf("***Starting Memory Test*** \r\n");
    for(int i=0; i<MAX_WORDS; i++) MEM_LOC[i] = i;

    for(int i=0; i<MAX_WORDS; i++) {
        if (MEM_LOC[i] != i) {
        bsp_printf("Data mismatched at address 0x%x with value of 0x%x \r\n", i, MEM_LOC[i]);
        while(1){
            }
        }
    }
    bsp_printf("Data matched .. Test PASSED\r\n");
    bsp_printf("***Succesfully Ran Demo*** \r\n");
}

