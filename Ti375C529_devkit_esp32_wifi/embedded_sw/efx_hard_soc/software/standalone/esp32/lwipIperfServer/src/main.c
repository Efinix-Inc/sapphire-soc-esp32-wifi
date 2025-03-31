////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "bsp.h"
#include "prescaler.h"
#include "timer.h"
#include "clint.h"
#include "riscv.h"
#include "plic.h"
#include "lwip/init.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "lwiperf.h"
#include "ethernetif_esp.h"


/******************************************************************/
#define TIMER_PRESCALER_CTRL            (TIMER_CTRL + 0x00)
#define TIMER_0_CTRL                    (TIMER_CTRL +  0x40)
#define TIMER_CONFIG_WITH_PRESCALER     0x2
#define TIMER_CONFIG_WITHOUT_PRESCALER  0x1
#define TIMER_CONFIG_SELF_RESTART       0x10000

/******************************************************************/

struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
ip4_addr_t client_addr;

#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 31
#define IP_ADDR3 55

#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 31
#define GW_ADDR3 1


/******************************************************************/


void trap_entry();
//Used on unexpected trap/interrupt codes
void crash(){
   bsp_printf("\n*** CRASH ***\n");
   while(1);
}

void isrRoutine(){
   uint32_t claim;
   // While there is pending interrupts
   while(claim = plic_claim(BSP_PLIC, BSP_PLIC_CPU_0)){
      switch(claim){
		  case SYSTEM_PLIC_USER_INTERRUPT_J_INTERRUPT:
			  got_esp_packet();
			break;
         
      default: crash(); break;
      }
      plic_release(BSP_PLIC, BSP_PLIC_CPU_0, claim); //unmask the claimed interrupt
   }
}

//Called by trap_entry on both exceptions and interrupts events
void trap(){
   int32_t mcause = csr_read(mcause);
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

void isrInit(){

    // Configure PLIC
    // Cpu 0 accept all interrupts with priority above 0
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0);

    // Enable SYSTEM_PLIC_USER_INTERRUPT_A_INTERRUPT rising edge interrupt
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_USER_INTERRUPT_J_INTERRUPT, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_USER_INTERRUPT_J_INTERRUPT, 1);

    // Enable interrupts
    // Set the machine trap vector (../common/trap.S)
    csr_write(mtvec, trap_entry);
    // Enable external interrupts
    csr_set(mie, MIE_MEIE);
    csr_write(mstatus, csr_read(mstatus) | MSTATUS_MPP | MSTATUS_MIE);
    bsp_printf("intr init done...\r\n");
}

u64_t sys_jiffies(void)
{
	u64 get_time;

	get_time = clint_getTime(BSP_CLINT);
    return ((get_time)/(BSP_CLINT_HZ/1000));
}

u64_t sys_now(void)
{
	u64 get_time;

	get_time = clint_getTime(BSP_CLINT);

	return ((get_time)/(BSP_CLINT_HZ/1000));

}

 
 void LwIP_Init(void)
 {

     IP4_ADDR(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
     IP4_ADDR(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
     IP4_ADDR(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3); 
     /* Initilialize the LwIP stack without RTOS */
     lwip_init();
     /* add the network interface (IPv4/IPv6) without RTOS */
     netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL,
               &ethernetif_init, &ethernet_input);
     /* Registers the default network interface */
     netif_set_default(&gnetif);
     if (netif_is_link_up(&gnetif))
     {
      	/*When the netif is fully configured this function must be called */
    	netif_set_up(&gnetif);

     }
     else
     {
         /* When the netif link is down this function must be called */
         netif_set_down(&gnetif);
     } 
 }

/****************************************************************MAIN**************************************************************/
void main() {
	
	int state;
	int check_connect;
	int bLink=0;

    bsp_printf("***Starting ESP32-S3 WiFi Demo***\n\r");
    //esp_commit();
    isrInit();

	LwIP_Init();

	lwiperf_start_tcp_server( &ipaddr, 5001, NULL, NULL );

	bsp_printf("iperf server Up\n\r\n\r");

	bsp_printf("=========================================\n\r");
	bsp_printf("======Lwip Raw Mode Iperf TCP Server ====\n\r");
	bsp_printf("=========================================\n\r");

	bsp_printf("======IP: \t\t");
	bsp_printf("%d",IP_ADDR0);
	bsp_printf(".");
	bsp_printf("%d",IP_ADDR1);
	bsp_printf(".");
	bsp_printf("%d",IP_ADDR2);
	bsp_printf(".");
	bsp_printf("%d",IP_ADDR3);
	bsp_printf("\n\r");

	bsp_printf("======Netmask: \t\t");
	bsp_printf("%d",NETMASK_ADDR0);
	bsp_printf(".");
	bsp_printf("%d",NETMASK_ADDR1);
	bsp_printf(".");
	bsp_printf("%d",NETMASK_ADDR2);
	bsp_printf(".");
	bsp_printf("%d",NETMASK_ADDR3);
	bsp_printf("\n\r");

	bsp_printf("======GateWay: \t\t");
	bsp_printf("%d",GW_ADDR0);
	bsp_printf(".");
	bsp_printf("%d",GW_ADDR1);
	bsp_printf(".");
	bsp_printf("%d",GW_ADDR2);
	bsp_printf(".");
	bsp_printf("%d",GW_ADDR3);
	bsp_printf("\n\r");


	bsp_printf("=========================================\n\r");
	

    while (1) {
    	if(check_esp_packet()) {
    		ethernetif_input(&gnetif);	//get ethernet input packet event
    	}
       	sys_check_timeouts();
    }
}
