///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "type.h"
#include "io.h"

#define SPI_DATA             0x00
#define SPI_BUFFER           0x04
#define SPI_CONFIG           0x08
#define SPI_INTERRUPT        0x0C
#define SPI_CLK_DIVIDER      0x20
#define SPI_SS_SETUP         0x24
#define SPI_SS_HOLD          0x28
#define SPI_SS_DISABLE       0x2C
#define SPI_WRITE_LARGE      0x50
#define SPI_READ_WRITE_LARGE 0x54
#define SPI_READ_LARGE       0x58
#define SPI_CMD_WRITE   (1 << 8)
#define SPI_CMD_READ    (1 << 9)
#define SPI_CMD_SS      (1 << 11)
#define SPI_RSP_VALID   (1 << 31)
#define SPI_STATUS_CMD_INT_ENABLE = (1 << 0)
#define SPI_STATUS_RSP_INT_ENABLE = (1 << 1)
#define SPI_STATUS_CMD_INT_FLAG = (1 << 8)
#define SPI_STATUS_RSP_INT_FLAG = (1 << 9)
#define SPI_MODE_CPOL   (1 << 0)
#define SPI_MODE_CPHA   (1 << 1)


    typedef struct {
        u32 cpol;
        u32 cpha;
        u32 mode;
        u32 clkDivider;
        u32 ssSetup;
        u32 ssHold;
        u32 ssDisable;
    } Spi_Config;
    
    static u32 spi_cmdAvailability(u32 reg){
        return read_u32(reg + SPI_BUFFER) & 0xFFFF;
    }
    static u32 spi_rspOccupancy(u32 reg){
        return read_u32(reg + SPI_BUFFER) >> 16;
    }
    
    static void spi_write(u32 reg, u8 data){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(data | SPI_CMD_WRITE, reg + SPI_DATA);
    }
    
    static u8 spi_read(u32 reg){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(SPI_CMD_READ, reg + SPI_DATA);
        while(spi_rspOccupancy(reg) == 0);
        return read_u32(reg + SPI_DATA);
    }
    
    static u8 spi_writeRead(u32 reg, u8 data){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(data | SPI_CMD_READ | SPI_CMD_WRITE, reg + SPI_DATA);
        while(spi_rspOccupancy(reg) == 0);
        return read_u32(reg + SPI_DATA);
    }

    static void spi_write32(u32 reg, u32 data){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(data, reg + SPI_WRITE_LARGE);
    }
    
    static u32 spi_writeRead32(u32 reg, u32 data){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(data, reg + SPI_READ_WRITE_LARGE);
        while(spi_rspOccupancy(reg) == 0);
        return read_u32(reg + SPI_READ_LARGE);
    }
    
    static u32 spi_read32(u32 reg){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(SPI_CMD_READ, reg + SPI_DATA);
        while(spi_rspOccupancy(reg) == 0);
        return read_u32(reg + SPI_READ_LARGE);
    }
    
    static void spi_select(u32 reg, u32 slaveId){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(slaveId | 0x80 | SPI_CMD_SS, reg + SPI_DATA);
    }
    
    static void spi_diselect(u32 reg, u32 slaveId){
        while(spi_cmdAvailability(reg) == 0);
        write_u32(slaveId | 0x00 | SPI_CMD_SS, reg + SPI_DATA);
    }
    
    static void spi_applyConfig(u32 reg, Spi_Config *config){
        write_u32((config->cpol << 0) | (config->cpha << 1) | (config->mode << 4), reg + SPI_CONFIG);
        write_u32(config->clkDivider, reg + SPI_CLK_DIVIDER);
        write_u32(config->ssSetup, reg + SPI_SS_SETUP);
        write_u32(config->ssHold, reg + SPI_SS_HOLD);
        write_u32(config->ssDisable, reg + SPI_SS_DISABLE);
    }
    
    /**
    * Wait for SPI Transfer to complete
    * 
    * @param reg SPI base address 
    */
    static void spi_waitXferBusy(u32 reg){
    	bsp_uDelay(1);
    	while(spi_cmdAvailability(reg) != 256);
    }

#if defined(DEFAULT_ADDRESS_BYTE) || defined(MX25_FLASH)
    /**
	* Read Status Register
	*
	* @param reg SPI base address
	* @param cs 32-bit bitwise chip select setting
	*/
    static u8 spiReadStatusRegister(u32 reg, u32 cs){
    	spi_select(reg, cs);
    	spi_write(reg, 0x05); //Read Status Register
    	u8 value = spi_read(reg);
    	spi_diselect(reg, cs);
    	return value;
    }

    /**
	* Write Status Register
	*
	* @param reg SPI base address
	* @param cs 32-bit bitwise chip select setting
	* @param data 8-bit data
	*/
    static void spiWriteStatusRegister(u32 reg, u32 cs, u8 data){

    	spi_select(reg, cs);
		spi_write(reg, 0x01); //Write Status Register
		spi_write(reg, data); //Write Status Register
		spi_diselect(reg, cs);
    }

    /**
	* Set Write Enable Latch
	*
	* @param reg SPI base address
	* @param cs 32-bit bitwise chip select setting
	*/
    static void spiWriteEnable(u32 reg, u32 cs){
		spi_select(reg, cs);
		spi_write(reg, 0x06); // Write Enable Sequence
		spi_diselect(reg, cs);
	}
#endif
