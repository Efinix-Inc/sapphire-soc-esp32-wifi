#include "esp32_spi_hd.h"

void __dump(u8 *data, u32 len) {
	const char *hs = "0123456789ABCDEF";
	for(int i = 0; i < len; i++) {
        uart_write(BSP_UART_TERMINAL, hs[data[i] >> 4]);
        uart_write(BSP_UART_TERMINAL, hs[data[i] & 0xf]);
	}
	uart_writeStr(BSP_UART_TERMINAL, "\r\n");
}

void m_flash_init(int mode) {
        Spi_Config spiCfg;
        spiCfg.cpol = 0;
        spiCfg.cpha = 0;
        spiCfg.mode = mode;
        spiCfg.clkDivider = SPI_CLOCK_DIV;
        spiCfg.ssSetup = 1;
        spiCfg.ssHold = 1;
        spiCfg.ssDisable = 4;
        spi_applyConfig(SPI, &spiCfg);
        spi_waitXferBusy(SPI);
}

void wr_buf(char *data, u8 addr, int size, int mode) {
    m_flash_init(0);
    spi_select(SPI, SPI_CS);
    spi_write(SPI, (mode << 4) | 0x1);
    spi_write(SPI, addr);
    spi_write(SPI, 0);
    spi_waitXferBusy(SPI);
    m_flash_init(mode);
    for(int i = 0; i < size; i++) {
    	spi_write(SPI, data[i]);
    }
    spi_waitXferBusy(SPI);
    m_flash_init(0);
    spi_diselect(SPI, SPI_CS);
}
void rd_buf(char *data, u8 addr, int size, int mode) {
    m_flash_init(0);
    spi_select(SPI, SPI_CS);
    spi_write(SPI, (mode << 4) | 0x2);
    spi_write(SPI, addr);
    spi_write(SPI, 0);
    spi_waitXferBusy(SPI);
    m_flash_init(mode);
    //printf("mode ===> %X\n", (mode << 4) | 2);
    for(int i = 0; i < size; i++) {
    	data[i] = (char)spi_read(SPI);
    }
    m_flash_init(0);
    spi_diselect(SPI, SPI_CS);
}

static void spi_write_dma(u32 reg, u32 addr, u32 size){
    write_u32(addr, reg + 0x64);
    write_u32(size, reg + 0x68);
    write_u32(1, reg + 0x60);
    while(read_u32(reg + 0x6c) != 0);
}

static void spi_read_dma(u32 reg, u32 addr, u32 size){
    write_u32(addr, reg + 0x64);
    write_u32(size, reg + 0x68);
    write_u32(0, reg + 0x60);
    while(read_u32(reg + 0x6c) != 0);
    data_cache_invalidate_all();
}

void wr_dma(char *data, int size, int mode) {
    m_flash_init(0);
	spi_select(SPI, SPI_CS);
    spi_write(SPI, (mode << 4) | 0x3);
    spi_write(SPI, 0);
    spi_write(SPI, 0);
    data_cache_invalidate_all();
    spi_waitXferBusy(SPI);
    m_flash_init(mode);
#if (ESP_DMA == 1)
    spi_write_dma(SPI, (u32)data, (u32)size);
#else
    for(int i = 0; i < size; i++) {
    	spi_write(SPI, data[i]);
    }
#endif
    m_flash_init(0);
    spi_diselect(SPI, SPI_CS);

	spi_select(SPI, SPI_CS);
    spi_write(SPI, 0x7);
    spi_write(SPI, 0);
    spi_diselect(SPI, SPI_CS);
}

void rd_dma(char *data, int size, int mode) {
    m_flash_init(0);
	spi_select(SPI, SPI_CS);
    spi_write(SPI, (mode << 4) | 0x4);
    spi_write(SPI, 0);
    spi_write(SPI, 0);
    spi_waitXferBusy(SPI);
    m_flash_init(mode);
#if (ESP_DMA == 1)
    spi_read_dma(SPI, (u32)data, (u32)size);
#else
    for(int i = 0; i < size; i++) {
    	data[i] = spi_read(SPI);
    }
#endif
    spi_waitXferBusy(SPI);

    m_flash_init(0);
    spi_diselect(SPI, SPI_CS);

	spi_select(SPI, SPI_CS);
    spi_write(SPI, 0x8);
    spi_write(SPI, 0);
    spi_diselect(SPI, SPI_CS);
}

static void esp_commit() {
    m_flash_init(0);
    spi_select(SPI, SPI_CS);
    spi_write(SPI, 9);
    spi_write(SPI, 0);
    spi_waitXferBusy(SPI);
    spi_diselect(SPI, SPI_CS);
}

u8 esp_check_status() {
	u32 status;
	rd_buf((u8 *)&status, 3, sizeof(status), ESP_SPI_MODE);
	return status;
}

void esp_wifi_start(esp_wifi_status_t *status) {
	status->cmd = 1;
	wr_buf((u8 *)status, 2, sizeof(esp_wifi_status_t), ESP_SPI_MODE);
	esp_commit();
}

void esp_wifi_stop(esp_wifi_status_t *status) {
	status->cmd = 0;
	wr_buf((u8 *)status, 2, sizeof(esp_wifi_status_t), ESP_SPI_MODE);
	esp_commit();
}

void esp_power_reset() {
    m_flash_init(0);
    spi_select(SPI, SPI_CS);
    spi_write(SPI, 10);
    spi_write(SPI, 0);
    spi_waitXferBusy(SPI);
    spi_diselect(SPI, SPI_CS);
}

