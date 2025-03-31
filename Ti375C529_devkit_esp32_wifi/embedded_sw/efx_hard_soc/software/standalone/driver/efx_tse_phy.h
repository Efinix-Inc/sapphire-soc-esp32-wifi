////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
// Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "bsp.h"
#include "userDef.h"


/************************** Function File ***************************/
static void Phy_Wr(u32 RegAddr, u32 Data)
{
    write_u32(((PHY_ADDR&0x1f)<<8)|(RegAddr&0x1f), (TSEMAC_BASE+REG_PHY_ADDR));
    write_u32(Data, (TSEMAC_BASE+WR_DATA));
    write_u32(0x2, (TSEMAC_BASE+RD_WR_EN));
    if(DEBUG_PRINTF_EN == 1) {
        bsp_printf("Wr Phy Addr :%x \r\n", RegAddr);
        bsp_printf("Wr Phy Data :%x \r\n ", Data);
    }
}

static u32 Phy_Rd(u32 RegAddr)
{
    u32 Value;
    write_u32(((PHY_ADDR&0x1f)<<8)|(RegAddr&0x1f), (TSEMAC_BASE+REG_PHY_ADDR));
    write_u32(0x1, (TSEMAC_BASE+RD_WR_EN));
    bsp_uDelay(1000);
    Value = read_u32(TSEMAC_BASE+RD_DATA);
    if(DEBUG_PRINTF_EN == 1) {
        bsp_printf("Rd Phy Addr :%x \r\n ", RegAddr);
        bsp_printf("Return Value :%x \r\n ", Value);
    }
    return Value;
}

/************************** Function File ***************************/
static void PhyDlySetRXTX(int RX_delay, int TX_delay)
{
    u32 Value;
    bsp_print("Start Info : Set Phy Delay.");
    Phy_Wr(0x1F,0x0168);
    Phy_Wr(0x1E,0x8040);
    Phy_Wr(0x1E,0x401E);
    Value = Phy_Rd(0x1F) & 0xFFFF;

    Value &= 0xFF00;
    RX_delay &= 0xF;
    TX_delay &= 0xF;
    bsp_printf("Setup New Value =%x \r\n ", RX_delay);

    Value = ((Value) | (RX_delay<<4) | (TX_delay));
    Phy_Wr(0x1F,Value);
    Phy_Wr(0x1E,0x801E);
    Phy_Wr(0x1E,0x401E);
    Value = Phy_Rd(0x1F) & 0xFFFF;
    bsp_printf("Read New Value =%x \r\n ", Value);
}

/************************** Function File ***************************/
static u32 PhyNormalInit()
{
	PhyDlySetRXTX(15, 8);

	u32 Value;
	if(DEBUG_PRINTF_EN == 1) {
		bsp_printf("Wait Ethernet Link up...");
	}
    // to read Basic control
	Phy_Rd(0x0); 
    // to read phy ID
	Phy_Rd(0x2); 
    // to read phy ID
	Phy_Rd(0x3); 

	//Unlock Extended registers
	Phy_Wr(0x1f, 0x0168);
	Phy_Wr(0x1e, 0x8040);

	while(1) {
		Value = Phy_Rd(0x11);
        //Link up and DUPLEX mode
		if((Value&0x2400) == 0x2400) {
			if((Value&0xc000) == 0x8000) {          //1000Mbps
				if(DEBUG_PRINTF_EN == 1) {
					bsp_printf("Info : Phy Link up on 1000Mbps.\r\n");
				}
				return 0x4;
			} else if((Value&0xc000) == 0x4000) {   //100Mbps
				if(DEBUG_PRINTF_EN == 1) {
					bsp_printf("Info : Phy Link up on 100Mbps.\r\n");
				}
				return 0x2;
			} else if((Value&0xc000) == 0x0) {      //10Mbps
				if(DEBUG_PRINTF_EN == 1) {
					bsp_printf("Info : Phy Link up on 10Mbps.\r\n");
				}
				return 0x1;
			}
		}
		bsp_uDelay(100000);
	}
}

/************************** Function File ***************************/
static void PhyLoopInit(u32 speed)
{
	PhyDlySetRXTX(15, 15);
	u32 Value;
	if(speed == 0x4) {
		Phy_Wr(0x0, 0x4140);
		if(DEBUG_PRINTF_EN == 1) {
			bsp_printf("Info : Set Phy 1000Mbps Loopback Mode.\r\n");
		}
	} else if(speed == 0x2) {
		Phy_Wr(0x0, 0x6100);
		if(DEBUG_PRINTF_EN == 1) {
			bsp_printf("Info : Set Phy 100Mbps Loopback Mode.\r\n");
		}
	} else if(speed == 0x1) {
		Phy_Wr(0x0, 0x4100);
		if(DEBUG_PRINTF_EN == 1) {
			bsp_printf("Info : Set Phy 10Mbps Loopback Mode.\r\n");
		}
	}
}

