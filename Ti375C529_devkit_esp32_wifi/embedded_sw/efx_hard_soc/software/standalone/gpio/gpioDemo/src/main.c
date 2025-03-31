////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "userDef.h"
#include "riscv.h"
#include "gpio.h"
#include "clint.h"
#include "plic.h"

void trap();
void crash();
void trap_entry();
void gpioIsr();
void isrRoutine();
void isrInit();
void main();

//Store the count of interrupt happened
u32 count;

// Used on unexpected trap/interrupt codes
void crash(){
    bsp_printf("\r\n*** CRASH ***\r\n");
    while(1);
}

// Called by trap_entry on both exceptions and interrupts events
void trap(){
    int32_t mcause = csr_read(mcause);
    int32_t interrupt = mcause < 0;    //Interrupt if true, exception if false
    int32_t cause     = mcause & 0xF;
    if(interrupt){
        switch(cause){
        case CAUSE_MACHINE_EXTERNAL: isrRoutine(); break;
        default: crash(); break;
        }
    } else {
        crash();
    }
}

void gpioIsr(){
    bsp_printf("Entering gpio interrupt routine .. \r\n");
    bsp_uDelay(100);
    count++;
    bsp_printf("Count:%d .. Done \r\n",count);
}

void isrRoutine(){
    uint32_t claim;
    // While there is pending interrupts
    while(claim = plic_claim(BSP_PLIC, BSP_PLIC_CPU_0)){
        switch(claim){
        case SYSTEM_PLIC_SYSTEM_GPIO_0_IO_INTERRUPTS_0: gpioIsr(); break;
        default: crash(); break;
        }
        // Unmask the claimed interrupt
        plic_release(BSP_PLIC, BSP_PLIC_CPU_0, claim); 
    }
}

void isrInit(){
    // Configure PLIC
    // Cpu 0 accept all interrupts with priority above 0
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0); 
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_GPIO_0_IO_INTERRUPTS_0, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_GPIO_0_IO_INTERRUPTS_0, 1);
    // Enable rising edge interrupts
    gpio_setInterruptRiseEnable(GPIO0, 1); 
    // Enable interrupts
    // Set the machine trap vector (../common/trap.S)
    csr_write(mtvec, trap_entry); 
    //Enable external interrupts
    csr_set(mie, MIE_MEIE); 
    csr_write(mstatus, csr_read(mstatus) | MSTATUS_MPP | MSTATUS_MIE);
}

void main() {
    bsp_init();
    bsp_printf("***Starting GPIO Demo*** \r\n");
    bsp_printf("Configure GPIOs to blink .. \r\n");
    // Set GPIO to output
    gpio_setOutputEnable(GPIO0, 0xe);
    gpio_setOutput(GPIO0, 0x0);
    for (int i=0; i<50; i=i+1) {
        gpio_setOutput(GPIO0, gpio_getOutput(GPIO0) ^ 0xe);
        bsp_uDelay(LOOP_UDELAY);
    }   
    bsp_printf("***Starting GPIO Interrupt Demo*** \r\n");
    bsp_printf("Press and release onboard button sw4 .. \r\n");
    isrInit();
    while(1); 
}


