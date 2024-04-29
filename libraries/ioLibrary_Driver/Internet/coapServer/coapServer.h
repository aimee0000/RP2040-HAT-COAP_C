#include <stdint.h>

#ifndef	__COAPSERVER_H__
#define	__COAPSERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define COAP_SERVER_PORT        5683

void coapServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t sock);
void coapServer_run();

#ifdef __cplusplus
}
#endif

#endif
