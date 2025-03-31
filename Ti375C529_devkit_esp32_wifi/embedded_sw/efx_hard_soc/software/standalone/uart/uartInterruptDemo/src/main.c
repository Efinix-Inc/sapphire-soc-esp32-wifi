////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "riscv.h"
#include "plic.h"
#include "clint.h"

void crash();
void trap();
void trap_entry();
void uartIsr();
void isrRoutine();
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

void uartIsr()
{
    if (uart_status_read(BSP_UART_TERMINAL) & 0x00000100){
        
        bsp_printf("\nEntering uart tx fifo empty interrupt routine .. \r\n");
        // Disable TX FIFO empty interrupt 
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) & 0xFFFFFFFE);  
        // Enable TX FIFO empty interrupt 
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) | 0x01);
        bsp_printf("Done .. \r\n"); 
    }
    else if (uart_status_read(BSP_UART_TERMINAL) & 0x00000200){

        bsp_printf("\nEntering uart rx fifo not empty interrupt routine .. \r\n");
        // Disable RX FIFO not empty interrupt 
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) & 0xFFFFFFFD);          
        // Read to clear RX FIFO
        uart_write(BSP_UART_TERMINAL, uart_read(BSP_UART_TERMINAL));    
        // Enable RX FIFO not empty interrupt 
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) | 0x02);
        bsp_printf("Done .. \r\n");                    
    }
}

void isrRoutine()
{
    uint32_t claim;
    // While there is pending interrupts
    while(claim = plic_claim(BSP_PLIC, BSP_PLIC_CPU_0)){
        switch(claim){
        case SYSTEM_PLIC_SYSTEM_UART_0_IO_INTERRUPT: uartIsr(); break;
        default: crash(); break;
        }
        // Unmask the claimed interrupt
        plic_release(BSP_PLIC, BSP_PLIC_CPU_0, claim); 
    }
}

void isrInit(){

    // TX FIFO empty interrupt enable
    //uart_TX_emptyInterruptEna(BSP_UART_TERMINAL,1);   
    
    // RX FIFO not empty interrupt enable
    uart_RX_NotemptyInterruptEna(BSP_UART_TERMINAL,1);  

    // Configure PLIC
    // Cpu 0 accept all interrupts with priority above 0
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0); 

    // Enable SYSTEM_PLIC_USER_INTERRUPT_A_INTERRUPT rising edge interrupt
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_UART_0_IO_INTERRUPT, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_UART_0_IO_INTERRUPT, 1);

    // Enable interrupts
    // Set the machine trap vector (../common/trap.S)
    csr_write(mtvec, trap_entry); 
    // Enable external interrupts
    csr_set(mie, MIE_MEIE); 
    csr_write(mstatus, csr_read(mstatus) | MSTATUS_MPP | MSTATUS_MIE);
}

void main() {
    bsp_init();
    isrInit();

    bsp_printf("***Starting Uart Interrupt Demo*** \r\n");
    bsp_printf("Start typing on terminal to trigger uart RX FIFO not empty interrupt .. \r\n");
    while(1){
        while(uart_readOccupancy(BSP_UART_TERMINAL)){
            uart_write(BSP_UART_TERMINAL, uart_read(BSP_UART_TERMINAL));
        }
    }
}


