///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "userDef.h"
#include "prescaler.h"
#include "timer.h"
#include "riscv.h"
#include "plic.h"

void trap();
void crash();
void trap_entry();
void isrRoutine();
void initTimer();
void isrInit();
void main();

// Used on unexpected trap/interrupt codes
void crash(){
    bsp_printf("\r\n*** CRASH ***\r\n");
    while(1);
}

// Called by trap_entry on both exceptions and interrupts events
void trap(){
    int32_t mcause = csr_read(mcause);
    // Interrupt if true, exception if false
    int32_t interrupt = mcause < 0;    
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

void isrRoutine(){
    uint32_t claim;

    // Save the Machine Exception Programme Counter to be able to restore it in case a higher priority interrupt happen
    u32 epc = csr_read(mepc);                                     
    // Save it to restore it later
    u32 threshold = plic_get_threshold(BSP_PLIC, BSP_PLIC_CPU_0); 

    // While there is pending interrupts
    while(claim = plic_claim(BSP_PLIC, BSP_PLIC_CPU_0)){
        // Identify which priority level the current claimed interrupt is
        u32 priority = plic_get_priority(BSP_PLIC, claim);            
        // Enable only the interrupts which higher priority than the currently claimed one.
        plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, priority);       
        // Enable machine external interrupts
        csr_set(mstatus, MSTATUS_MIE);                                
        switch(claim){
        case SYSTEM_PLIC_TIMER_INTERRUPTS_0: {
            bsp_printf("T0S-HP\r\n");
            bsp_printf("T0E-HP\r\n");
        }  break;
        case SYSTEM_PLIC_TIMER_INTERRUPTS_1: {
            bsp_printf("T1S\r\n");
            // User delay
            for(int i = 0;i < 1200000;i++) asm("nop"); 
            bsp_printf("T1E\r\n");
            // That timer wasn't configured with self restart, require to do it manually.
            timer_clearValue(TIMER_1_CTRL); 
        } break;
        default: crash(); break;
        }
        // disable machine external interrupts
        csr_clear(mstatus, MSTATUS_MIE);                              
        // Restore the original threshold level
        plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, threshold); 
        plic_release(BSP_PLIC, BSP_PLIC_CPU_0, claim); //unmask the claimed interrupt
    }
    //Restore the mepc, in case it was overwritten by a nested interrupt
    csr_write(mepc, epc); 
}

void initTimer(){
    // Divide clock rate by 399+1
    prescaler_setValue(TIMER_0_PRESCALER_CTRL, 399); 
    // Will tick each (9999+1)*(399+1) cycles (as it use the prescaler)
    timer_setLimit(TIMER_0_CTRL, 3999);
    timer_setConfig(TIMER_0_CTRL, TIMER_CONFIG_WITH_PRESCALER | TIMER_CONFIG_SELF_RESTART); 
    // Will tick each (3999+1)*(999+1) cycles (as it use the prescaler)
    prescaler_setValue(TIMER_1_PRESCALER_CTRL, 999); 
    timer_setLimit(TIMER_1_CTRL, 3999);
    timer_setConfig(TIMER_1_CTRL, TIMER_CONFIG_WITH_PRESCALER); 
}

void isrInit(){
    // Configure timer
    initTimer();
    // Configure PLIC
    // Cpu 0 accept all interrupts with priority above 0
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0); 
    // Enable Timer 0 interrupts
    // Priority 2 win against priority 1
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_TIMER_INTERRUPTS_0, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_TIMER_INTERRUPTS_0, 2);  
    // Enable Timer 1 interrupts
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_TIMER_INTERRUPTS_1, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_TIMER_INTERRUPTS_1, 1);
    // Enable interrupts
    // Set the machine trap vector (../common/trap.S)
    csr_write(mtvec, trap_entry); 
    // Enable external interrupts only
    csr_set(mie, MIE_MEIE); 
    csr_write(mstatus, csr_read(mstatus) | MSTATUS_MPP | MSTATUS_MIE);
}

void main() {
    bsp_init();
    bsp_printf("***Starting Nested Interrupt Demo*** \r\n");
    isrInit();
    while(1); 
}

