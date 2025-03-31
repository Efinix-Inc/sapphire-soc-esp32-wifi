///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#include "bsp.h"
#include "userDef.h"
#include "spi.h"

void WaitBusy(void)
{
    u8 out;
    u16 timeout=0;

    while(1)
    {
        bsp_uDelay(1*1000);
        spi_select(SPI, 0);
        //Write Enable
        spi_write(SPI, 0x05);   
        out = spi_read(SPI);
        spi_diselect(SPI, 0);
        if((out & 0x01) ==0x00)
            return;
        timeout++;
        //sector erase max=400ms
        if(timeout >=400)       
        {
            bsp_printf("Time out .. \r\n");
            return;
        }
    }
}

void WriteEnableLatch(void)
{
    spi_select(SPI, 0);
    //Write Enable latch
    spi_write(SPI, 0x06);   
    spi_diselect(SPI, 0);
}

void GlobalLock(void)
{
    WriteEnableLatch();
    spi_select(SPI, 0);
    //Global lock
    spi_write(SPI, 0x7E);   
    spi_diselect(SPI, 0);
}

void GlobalUnlock(void)
{
    WriteEnableLatch();
    spi_select(SPI, 0);
    //Global unlock
    spi_write(SPI, 0x98);   
    spi_diselect(SPI, 0);
}

void SectorErase(u32 Addr)
{
    WriteEnableLatch();
    spi_select(SPI, 0);     
    //Erase Sector
    spi_write(SPI, 0x20);
    spi_write(SPI, (Addr>>16)&0xFF);
    spi_write(SPI, (Addr>>8)&0xFF);
    spi_write(SPI, Addr&0xFF);
    spi_diselect(SPI, 0);
    WaitBusy();
}

void spiInit(){
    //SPI init
    Spi_Config spiA;
    spiA.cpol       = 1;
    spiA.cpha       = 1;
    spiA.mode       = 0; 
    spiA.clkDivider = 19;
    spiA.ssSetup    = 5;
    spiA.ssHold     = 5;
    spiA.ssDisable  = 5;
    spi_applyConfig(SPI, &spiA);
}

void main() {
    uint8_t id;
    int i,len;
    u8 out;

    bsp_init();
    bsp_printf("***Starting SPI Demo*** \r\n");
    spiInit();
    spi_select(SPI, 0);
    spi_write(SPI, 0xAB);
    spi_write(SPI, 0x00);
    spi_write(SPI, 0x00);
    spi_write(SPI, 0x00);
    id = spi_read(SPI);
    spi_diselect(SPI, 0);
    bsp_printf("Device ID : %x \r\n", id);

       
    bsp_printf("Writing data to flash .. \r\n");
    len=256;
    GlobalUnlock();
    SectorErase(FLASH_START_ADDR);
    WriteEnableLatch();
    spi_select(SPI, 0);
    spi_write(SPI, 0x02);
    spi_write(SPI, (FLASH_START_ADDR>>16) & 0xFF);
    spi_write(SPI, (FLASH_START_ADDR>>8) & 0xFF);
    spi_write(SPI, FLASH_START_ADDR & 0xFF);
    // Write dummy data
    for(i=0; i<len; i++)          
    {
        spi_write(SPI, i & 0xFF);
        bsp_printf("Write address %x := %x \r\n", FLASH_START_ADDR+i, i & 0xFF);
    }
    spi_diselect(SPI, 0);
    // Wait for page writing done
    WaitBusy(); 
    GlobalLock();
   
    bsp_printf("Reading from flash .. \r\n");
    for(i=FLASH_START_ADDR;i< (FLASH_START_ADDR+len) ;i++)
    {
        spi_select(SPI, 0);
        spi_write(SPI, 0x03);
        spi_write(SPI, (i>>16) & 0xFF);
        spi_write(SPI, (i>>8) & 0xFF);
        spi_write(SPI, i & 0xFF);
        out = spi_read(SPI);
        spi_diselect(SPI, 0);
        bsp_printf("Read address %x := %x \r\n", i, out);
    }
    bsp_printf("***Succesfully Ran Demo*** \r\n");
}

