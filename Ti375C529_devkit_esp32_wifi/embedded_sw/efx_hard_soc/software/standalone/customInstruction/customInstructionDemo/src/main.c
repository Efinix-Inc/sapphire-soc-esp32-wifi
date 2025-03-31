////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "userDef.h"
#include "riscv.h"

void error_state() {
    bsp_printf("Custom instruction and software output results are not matched .. \r\n");
    while (1) {}
}

void software_tinyEncrypt (uint32_t v0, uint32_t v1, uint32_t *rv0, uint32_t *rv1) {
    uint32_t sum=0, i;

    uint32_t delta=0x9e3779b9;

    uint32_t k0=0x01234567,
             k1=0x89abcdef,
             k2=0x13579248,
             k3=0x248a0135;

    for (i=0; i < 1024; i++) {
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);

    }
    *rv0=v0;
    *rv1=v1;
}

void printPTime(uint64_t ts1, uint64_t ts2, char *s) {
    uint64_t rts;
    rts=ts2-ts1;
    bsp_printf("%s %d \n\n\r", s, rts);

}

void main() {
    uint32_t num1, num2;
    uint64_t timerCmp0, timerCmp1;
    uint32_t result_ci0,result_ci1,result_s0,result_s1;

    num1=0x84425820;
    num2=0xdeadbe11;

    bsp_init();
    bsp_printf("***Starting Custom Instruction Demo*** \r\n");
    timerCmp0 = clint_getTime(BSP_CLINT);
    result_ci0=tinyEncryption_lowerword(num1,num2);
    result_ci1=tinyEncryption_upperword(0x0, 0x0);
    timerCmp1 = clint_getTime(BSP_CLINT);
    bsp_printf("Custom instruction method");
    printPTime(timerCmp0,timerCmp1," processing clock cycles:");

    timerCmp0 = clint_getTime(BSP_CLINT);
    software_tinyEncrypt(num1, num2, &result_s0, &result_s1);
    timerCmp1 = clint_getTime(BSP_CLINT);
    bsp_printf("Software method");
    printPTime(timerCmp0,timerCmp1," processing clock cycles:");

    if(result_ci0 != result_s0 || result_ci1 != result_s1) {
        error_state();
    } else {
        bsp_printf("Custom instruction and software output results are matched .. \r\n");
    }

    bsp_printf("***Succesfully Ran Demo*** \r\n");
}
