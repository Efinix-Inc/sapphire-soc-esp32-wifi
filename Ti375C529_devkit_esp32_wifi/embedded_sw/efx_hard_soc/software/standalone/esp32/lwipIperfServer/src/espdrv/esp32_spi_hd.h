#ifndef __ESP32_SPI_HD_H__
#define __ESP32_SPI_HD_H__
#include "bsp.h"
#include "io.h"
#include "spiFlash.h"

#define SPI IO_APB_SLAVE_1_INPUT	//APB3 bus for this ip
#define SPI_CS 			0			//spi id
#define SPI_CLOCK_DIV 	3			//SPI SCLK Clock: IP main clock / SPI_CLOCK_DIV
#define ESP_SPI_MODE 	2 			//0: full-duplex, 1: dual half-duplex, 2: quad half-duplex
#define ESP_DMA 		1			//0: disable dma, 1: use dma

#define WIFI_MODE 0 				//0: STA, 1: AP
#define WIFI_SSID "hello" 			//max length 32
#define WIFI_PASS "12345678" 		//max length 16
#define WIFI_MAC 0x001122334455ULL 	//mac address

void wr_buf(char *data, u8 addr, int size, int mode);
void rd_buf(char *data, u8 addr, int size, int mode);
void wr_dma(char *data, int size, int mode);
void rd_dma(char *data, int size, int mode);

struct __esp_wifi_status_t {
	u8 cmd; 			//1: start, 0: stop
	u8 connected;		//1: connected, 0: disconnected
	u16 mode;			//1: ap mode,	0: sta mode
	u8  ssid[32];		//wifi ssid max 32 bytes
	u8  pw[16];			//wifi password max 16 bytes;
} __attribute((packed));
typedef struct __esp_wifi_status_t esp_wifi_status_t;

u8 esp_check_status();									//check esp32 wifi connection status
void esp_wifi_start(esp_wifi_status_t *status);			//start connection
void esp_wifi_stop(esp_wifi_status_t *status);			//stop connection
void esp_power_reset();									//soft reset esp32

void __dump(u8 *data, u32 len);
#endif
