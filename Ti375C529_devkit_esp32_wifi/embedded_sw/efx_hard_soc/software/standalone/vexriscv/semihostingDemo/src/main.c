////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"

void main() {
#if (ENABLE_SEMIHOSTING_PRINT == 1)
    uint8_t dat;
    uint8_t string[100];
    uint8_t counter = 0;

    bsp_init();
    bsp_printf("***Starting Semihosting Demo ***\n\r");
    bsp_printf("You should see this printing in your console .. \n\r");
    bsp_printf("Echo demo. Key in your string and press enter .. \n\r");
    while (1){
        dat=sh_readc();
        if (dat != '\n'){
            string[counter]=dat;
            counter++;
        }
        else {
            string[counter]='\0';
            bsp_printf("Echo string: %s \r\n", string);
            counter = 0;
        }
    }

#else
    #error "Set ENABLE_SEMIHOSTING_PRINT to 1 in bsp.h for the program to work as expected..."
#endif 

}
