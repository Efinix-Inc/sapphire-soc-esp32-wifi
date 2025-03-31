# Wi-Fi Demo Solution

## Overview
This project demonstrates a Wi-Fi connectivity solution using the Efinix Titanium Ti375C529 Development Kit interfaced with the ESP32-S3-DevKitM-1. The solution enables wireless connectivity for FPGA-based applications through the powerful ESP32-S3 Wi-Fi module.

## Hardware Requirements
- [Efinix Titanium Ti375C529 Development Kit](https://www.efinixinc.com/products-devkits-titaniumti375c529.html)
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/ti375c529-dev-kit.PNG?raw=true)
- [ESP32-S3-DevKitM-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitm-1/index.html)
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/esp32-s3-devkitm-1.PNG?raw=true)
- USB cables for programming and powering both devices
- Jumper wires for connections between the FPGA and ESP32

## Software Requirements
- [Efinity IDE](https://www.efinixinc.com/support/efinity.php) (v2024.2.294.4.15 or later)
- [Efinity RISC-V Embedded Software IDE](https://www.efinixinc.com/support/efinity.php) (v2024.2.0.1 or later)
- [ESP-IDF](https://dl.espressif.com/dl/esp-idf/?idf=4.4) (v5.4 or later)

## Project Structure
```
sapphire-soc-esp32-wifi/
├── Ti375C529_devkit_esp32_wifi/        # FPGA design files
│   ├── embedded_sw/                    # RISC-V firmware
│   ├── ip/                             # IP cores
│   └── source/                         # RTL source files
├── doc/                                # Documentation
├── esp32/                              # ESP32-S3 firmware
│   └── wifi_bridge/                    # Main application
├── iperf/                              # iPerf2 application
└── README.md                           # This file
```

## Design Architecture
- FPGA will access MAC layer data in ESP32 via SPI buses
- When ESP32 received FPGA MAC data, it will MAC information to the ESP32 Wi-Fi baseband, then it will send out the data packet
- When ESP32 received the WO-FI baseband data, it will decode them and notify the FPGA application, finally it will perform DMA R/W via SPI buses
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/design-architecture.PNG?raw=true)

## Hardware Setup
1. Connect the ESP32-S3-DevKitM-1 to the Efinix Titanium board using the following pins:

| Efinix Ti375C529 Pin | PMOD_A Pin           | ESP32-S3 Pin                            | Function    |
|----------------------|----------------------|-----------------------------------------|-------------|
| PMOD_A_IO_0          | 1                    | 12                                      | SPI_MOSI    |
| PMOD_A_IO_1          | 2                    | 13                                      | SPI_MISO    |
| PMOD_A_IO_2          | 3                    | 11                                      | SPI_HD      |
| PMOD_A_IO_3          | 4                    | 10                                      | SPI_WP      |
| GND                  | 5                    | G (Use the one same column with 3V3)    | Ground      |
| 3V3                  | 6                    | 3V3                                     | Power       |
| PMOD_A_IO_4          | 7                    | 15                                      | SPI_SCLK    |
| PMOD_A_IO_5          | 8                    | 14                                      | SPI_SS      |
| PMOD_A_IO_6          | 9                    | 2                                       | ESP32_READY |

2. Connect USB1 of Ti375C529 dev kit to your computer for UART and programming.
3. Connect UART of ESP32-S3-DevKitM-1 dev kit to your computer for UART and programming.

## Software Setup
1. In this demo, we will use the built-in Windows `Mobile hotspot`.
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/mobile-hotspot.png?raw=true)
3. Edit the Hotspot setting
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/mobile-hotspot-setting.png?raw=true)
4. Turn on the Hotspot
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/mobile-hotspot-on.png?raw=true)
5. Edit the connection
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/network-connections.png?raw=true)
6. Edit the ipv4 properties
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/ipv4-properties.png?raw=true)

## Building, Flashing and Running

### ESP32-S3
1. Copy `esp32\wifi_bridge` to `\frameworks\esp-idf-v5.4\examples\peripherals\spi_slave_hd\`
2. Launch `ESP-IDF 5.4 CMD`
```
idf.py set-target esp32s3
idf.py build flash monitor
```
3. If everything is okay, you should be something like this:
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/esp32-ready.PNG?raw=true)

### FPGA
1. Open the Efinity RISC-V IDE and import the project from the `Ti375C529_devkit_esp32_wifi/` directory
3. Run synthesis, place and route
4. Program the FPGA using Efinity Programmer
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/efinity-programmer.PNG?raw=true)
   
### RISC-V
1. Open the Efinity IDE and import the project from the `sapphire-soc-esp32-wifi/Ti375C529_devkit_esp32_wifi/embedded_sw` directory
2. Build and launch the application through openOCD
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/launch-openocd.png?raw=true)
3. If the RISC-V application is running, you will see the following output at the serial terminal
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/riscv-ready.PNG?raw=true)
4. If everyhing is okay, you should see something like this at ESP-IDF CMD prompt:
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/wifi-connected.PNG?raw=true)
5. If you see the following output on ESP-IDF CMD prompt, the connection is failed.
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/wifi-disconnected.PNG?raw=true)
6. If the connection is failed, stop open OCD and repeat step 2
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/stop-openocd.png?raw=true)

## Demo Applications
Basic Connectivity: Simple ping test to verify network connection
```
ping 192.168.31.55
```
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/ping-okay.PNG?raw=true)

Data Streaming: Continuous data transfer to measure throughput
```
cd sapphire-soc-esp32-wifi\iperf
iperf.exe -c 192.168.31.55 -i 1
```
- ![alt text](https://github.com/Efinix-Inc/sapphire-soc-esp32-wifi/blob/main/doc/iperf.PNG?raw=true)


## Customization
Edit esp32/main/wifi_config.h to change default Wi-Fi settings
Modify fpga/rtl/command_processor.sv to implement custom commands
Add new features to the web interface in esp32/main/http_server.c

## Troubleshooting
ESP32 not connecting: Check Wi-Fi credentials in the configuration
FPGA not responding: Verify pin connections and power supply
Communication errors: Check baud rate and SPI settings
Low performance: Adjust buffer sizes and clock frequencies

## Performance Metrics
Wi-Fi Throughput: Up to 20 Mbps (typical)
Latency: 5-10 ms (typical)

## Acknowledgments
Contributors to open-source libraries used in this project

## Contact
For questions, issues, or contributions, please open an issue in the project repository.
