////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////

#include "bsp.h"
#include "riscv.h"
#include <math.h>

void main() {
    double inp1,
           inp2,
           rSin,
           rCos,
           rTan,
           rSqrt,
           rDiv;    

    bsp_init();
    bsp_printf("***Starting FPU Demo*** \r\n");

    inp1=-0.8414709848078965;      
    rSin=sin(inp1);
    rCos=cos(inp1);
    rTan=tan(inp1);
    
    inp2=0.4161468365471424;
    rSqrt=sqrt(inp2);
    rDiv=inp2/3.6789;

    bsp_printf("\r\n");
    bsp_printf("Input 1 (in rad): %f \r\n", inp1);
    bsp_printf("Sine result: %f \r\n", rSin);
    bsp_printf("Cosine result: %f \r\n", rCos);
    bsp_printf("Tangent result: %f \r\n", rTan);
    bsp_printf("Input 2: %f \r\n", inp2);
    bsp_printf("Square root result: %f \r\n", rSqrt);
    bsp_printf("Divsion result: %f \r\n", rDiv);

    bsp_printf("***Succesfully Ran Demo*** \r\n");
}
