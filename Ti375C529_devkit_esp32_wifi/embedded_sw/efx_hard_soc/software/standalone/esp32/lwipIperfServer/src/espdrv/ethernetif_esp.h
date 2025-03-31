//HIHI ethernetif
#ifndef LWIP_ETHERNETIF_H
#define LWIP_ETHERNETIF_H


#include "lwip/err.h"
#include "lwip/netif.h"
#include "dmasg.h"

#define FRAME_PACKET  	256
#define BUFFER_SIZE 	1514

err_t ethernetif_init(struct netif *netif);
void  ethernetif_input(struct netif *netif);
void got_esp_packet();
int check_esp_packet();


#endif /* LWIP_ETHERNETIF_H */
