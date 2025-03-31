#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/spi_slave_hd.h"
#include "soc/spi_reg.h"
#include "driver/gpio.h"
#include "hal/spi_types.h"

#include "esp_mac.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_private/wifi.h"

#include "esp_ipc.h"
#include "esp_ipc_isr.h"
#include "esp_system.h"
#include "wired_iface.h"


#ifdef CONFIG_IDF_TARGET_ESP32
#define SLAVE_HOST    HSPI_HOST
#else
#define SLAVE_HOST    SPI2_HOST
#endif

#define GPIO_HANDSHAKE      2
#define GPIO_MOSI           12
#define GPIO_MISO           13
#define GPIO_SCLK           15
#define GPIO_CS             14
#define GPIO_WP             10
#define GPIO_HD             11

static const char *TAG = "esp_wifi_bridge";

#define CONFIG_EXAMPLE_MAX_STA_CONN 3
#define CONFIG_EXAMPLE_WIFI_CHANNEL 7
#define WIFI_CONNECTED_REG   0x03


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct __esp_wifi_status_t {
	u8 cmd; 			//1: start, 0: stop
	u8 connected;		//1: connected, 0: disconnected
	u16 mode;			//1: ap mode,	0: sta mode
	u8  ssid[32];		//wifi ssid max 32 bytes
	u8  pw[16];			//wifi password max 16 bytes;
} __attribute((packted));

typedef struct __esp_wifi_status_t esp_wifi_status_t;
static esp_wifi_status_t wifi_status;
static uint8_t s_sta_mac[6];
static SemaphoreHandle_t xCmdSemaphore;
static int wifi_mode = 0;
static int mac_configured = 0;

static esp_err_t pkt_wifi2eth(void *buffer, uint16_t len, void *eb)
{
    spi_slave_hd_data_t *ret_trans;
    spi_slave_hd_data_t tx_trans;
    tx_trans.data = buffer;
    tx_trans.len = len;

    spi_slave_hd_write_buffer(SLAVE_HOST, 0, (uint8_t *)&len, sizeof(len));
    ESP_ERROR_CHECK(spi_slave_hd_queue_trans(SLAVE_HOST, SPI_SLAVE_CHAN_TX, &tx_trans, portMAX_DELAY));
    ESP_ERROR_CHECK(spi_slave_hd_get_trans_res(SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
    
    esp_wifi_internal_free_rx_buffer(eb);

    return ESP_OK;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id) {
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a AP connected");
            mac_configured = 0;
            esp_wifi_internal_reg_rxcb(WIFI_IF_AP, pkt_wifi2eth);
            wifi_status.connected = 1;
            spi_slave_hd_write_buffer(SLAVE_HOST, WIFI_CONNECTED_REG, (uint8_t *)&wifi_status.connected, 1);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a AP disconnected");
            wifi_status.connected = 0;
            spi_slave_hd_write_buffer(SLAVE_HOST, WIFI_CONNECTED_REG, (uint8_t *)&wifi_status.connected, 1);
            esp_wifi_internal_reg_rxcb(WIFI_IF_AP, NULL);
            break;
        case WIFI_EVENT_STA_CONNECTED:
            wifi_status.connected = 1;
            mac_configured = 0;
            ESP_LOGI(TAG, "Wi-Fi STA got a station connected");
            spi_slave_hd_write_buffer(SLAVE_HOST, WIFI_CONNECTED_REG, (uint8_t *)&wifi_status.connected, 1);
            esp_wifi_internal_reg_rxcb(WIFI_IF_STA, pkt_wifi2eth);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            wifi_status.connected = 0;
            spi_slave_hd_write_buffer(SLAVE_HOST, WIFI_CONNECTED_REG, (uint8_t *)&wifi_status.connected, 1);
            ESP_LOGI(TAG, "Wi-Fi STA got a station disconnected");
            esp_wifi_internal_reg_rxcb(WIFI_IF_STA, NULL);
            break;        
        default:
            break;
    }
}

static void initialize_wifi(void)
{
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
    const char *mssid = "hello";
    const char *mpwd = "12345678";
    strncpy((char *)wifi_config.sta.ssid, mssid, 32);
    strncpy((char *)wifi_config.sta.password, mpwd, 16);

    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_connect();
    esp_wifi_set_ps(WIFI_PS_NONE);
}
static bool cb_set_tx_ready(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    gpio_set_level(GPIO_HANDSHAKE, 0);
    return true;
}

static bool cb_set_rx_ready(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    return true;
}

static bool cb_sent(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    uint16_t len = 0;
    spi_slave_hd_write_buffer(SLAVE_HOST, 0, (uint8_t *)&len, sizeof(len));
    return true;
}

static bool cb_recv(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    return true;
}

static bool cb_cmd9(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    BaseType_t status = pdFALSE;
    xSemaphoreGiveFromISR(xCmdSemaphore, &status);
    return true;
}

static bool cb_cmda(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    esp_restart();
    return true;
}

static bool cb_buffer_rx(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    spi_slave_hd_read_buffer(SLAVE_HOST, 2, (uint8_t *)&wifi_status, sizeof(esp_wifi_status_t));
    return true;
}

static bool cb_buffer_tx(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    gpio_set_level(GPIO_HANDSHAKE, 1);
    return true;
}
static void init_slave_hd(void)
{
    spi_bus_config_t bus_cfg = {};

    memset(&bus_cfg, 0x0, sizeof(spi_bus_config_t));
    bus_cfg.mosi_io_num = GPIO_MOSI;
    bus_cfg.miso_io_num = GPIO_MISO;
    bus_cfg.sclk_io_num = GPIO_SCLK;
    bus_cfg.quadwp_io_num = GPIO_HD;
    bus_cfg.quadhd_io_num = GPIO_WP;
    bus_cfg.max_transfer_sz = 2048;
    bus_cfg.flags = SPI_DEVICE_HALFDUPLEX;
    bus_cfg.intr_flags = 0;

    spi_slave_hd_slot_config_t slave_hd_cfg = {};

    memset(&slave_hd_cfg, 0x0, sizeof(spi_slave_hd_slot_config_t));
    slave_hd_cfg.spics_io_num = GPIO_CS;
    slave_hd_cfg.flags = 0;
    slave_hd_cfg.mode = 0;
    slave_hd_cfg.command_bits = 8;
    slave_hd_cfg.address_bits = 8;
    slave_hd_cfg.dummy_bits = 8;
    slave_hd_cfg.queue_size = 16;
    slave_hd_cfg.dma_chan = SPI_DMA_CH_AUTO;
    slave_hd_cfg.cb_config = (spi_slave_hd_callback_config_t) {
        .cb_send_dma_ready = cb_set_tx_ready,
        .cb_recv_dma_ready = cb_set_rx_ready,
        .cb_sent = cb_sent,
        .cb_recv = cb_recv,
        .cb_cmd9 = cb_cmd9,
        .cb_cmdA = cb_cmda,
        .cb_buffer_rx = cb_buffer_rx,
        .cb_buffer_tx = cb_buffer_tx
    };
    
    ESP_ERROR_CHECK(spi_slave_hd_init(SLAVE_HOST, &bus_cfg, &slave_hd_cfg));
}

static void cmd_task(void *args) {
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    esp_wifi_set_ps(WIFI_PS_NONE);

    memset(&wifi_status, 0, sizeof(esp_wifi_status_t));
    wifi_config_t wifi_config;
    u8 *data = (u8 *)&wifi_status;
    while (1) {        
        if (xSemaphoreTake(xCmdSemaphore, portMAX_DELAY) == pdTRUE) {
            if(!wifi_status.cmd) { //stop
                if(!wifi_status.mode)
                    esp_wifi_disconnect();
                esp_wifi_stop();
                continue;
            }
            if(wifi_status.cmd && wifi_status.connected) {
                mac_configured = 0;
                continue;
            }
            esp_wifi_get_config(wifi_status.mode ? WIFI_MODE_AP : WIFI_IF_STA, &wifi_config);
            int ssid_len = strlen((char *)wifi_status.ssid);
            int pw_len = strlen((char *)wifi_status.pw);
            if(!wifi_status.mode) {
                strncpy((char *)wifi_config.sta.ssid, (char *)wifi_status.ssid, 32);
                strncpy((char *)wifi_config.sta.password, (char *)wifi_status.pw, 16);
            } else {
                wifi_config.ap.ssid_len = ssid_len > 32 ? 32 : ssid_len;
                strncpy((char *)wifi_config.ap.ssid, (char *)wifi_status.ssid, 32);
                strncpy((char *)wifi_config.ap.password, (char *)wifi_status.pw, 16);
            }
        
            if (wifi_status.mode && pw_len == 0) {
                wifi_config.ap.authmode = WIFI_AUTH_OPEN;
            }
            printf("mode: %d, wifi ssid: %s, pw: %s\n", wifi_status.mode, (char *)wifi_status.ssid, (char *)wifi_status.pw);
            size_t free_heap_size = esp_get_free_heap_size();
            ESP_LOGI(TAG, "Free heap size before Wi-Fi start: %d bytes", free_heap_size);
            ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_status.mode ? WIFI_MODE_AP : WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(wifi_status.mode ? WIFI_IF_AP : WIFI_IF_STA, &wifi_config));
            ESP_ERROR_CHECK(esp_wifi_start());
            if(!wifi_status.mode) {
                esp_wifi_connect();
            }
            
        }
    }
}

static void recv_data_task(void *args)    
{       
    spi_slave_hd_data_t *ret_trans;
    spi_slave_hd_data_t rx_trans;
    uint8_t *rx_buf = (uint8_t *)malloc(2048);
    rx_trans.data = rx_buf;
    rx_trans.len = 2048;

    while(1) {
        //200 / portTICK_PERIOD_MS
        ESP_ERROR_CHECK(spi_slave_hd_queue_trans(SLAVE_HOST, SPI_SLAVE_CHAN_RX, &rx_trans, portMAX_DELAY));
        ESP_ERROR_CHECK(spi_slave_hd_get_trans_res(SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
        esp_wifi_internal_tx(wifi_status.mode ? ESP_IF_WIFI_AP : ESP_IF_WIFI_STA, ret_trans->data, ret_trans->trans_len);
        if(!mac_configured) {
            printf("confirue MAC to:");
            for(int i = 0; i < 6; i++) {
                printf("%.2X", ret_trans->data[i+6]);
            }
            printf("\n");
            esp_wifi_set_mac(wifi_status.mode ? ESP_IF_WIFI_AP : ESP_IF_WIFI_STA, &ret_trans->data[6]);
            mac_configured = 1;
        }
    }
    
    vTaskDelete(NULL);    
}     

void app_main(void)
{
    init_slave_hd();
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(GPIO_HANDSHAKE),
    };
    //Configure handshake line as output
    xCmdSemaphore = xSemaphoreCreateBinary();
    gpio_config(&io_conf);    
    gpio_set_level(GPIO_HANDSHAKE, 1);    

    uint8_t *buf = (uint8_t *)malloc(64);
    spi_slave_hd_write_buffer(SLAVE_HOST, 0, buf, 64);
    free(buf);

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    int ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ret = nvs_flash_init();
    }
    
    xTaskCreatePinnedToCore(recv_data_task, "recvTask", 4096, 0, 1, NULL, 1);
    xTaskCreatePinnedToCore(cmd_task,       "cmdTask",  4096, 0, 1, NULL, 0);
}