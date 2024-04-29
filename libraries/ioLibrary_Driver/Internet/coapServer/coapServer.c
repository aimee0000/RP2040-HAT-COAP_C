#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "coapServer.h"
#include "coap.h"

#include "socket.h"
#include "wizchip_conf.h"

#define DATA_BUF_SIZE		2048

uint8_t * pCOAP_TX;
uint8_t * pCOAP_RX;

static uint8_t COAPSock_Num = 0;

static void coapServer_Sockinit(uint8_t sock);

void coapServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t sock);
void coapServer_run();

static void coapServer_Sockinit(uint8_t sock)
{
	uint8_t i;

    COAPSock_Num = sock;

}

void coapServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t sock)
{
	// User's shared buffer
	pCOAP_TX = tx_buf;
	pCOAP_RX = rx_buf;

	// H/W Socket number mapping
	coapServer_Sockinit(sock);
}

void coapServer_run()
{
    int32_t ret, n;
    coap_packet_t pkt;
    uint16_t size = 0, sentsize=0;
    uint8_t scratch_raw[DATA_BUF_SIZE];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

    uint8_t  destip[4];
    uint16_t destport;


   switch(getSn_SR(COAPSock_Num))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(COAPSock_Num)) > 0)
         {
            if(size > DATA_BUF_SIZE) 
                size = DATA_BUF_SIZE;
            ret = recvfrom(COAPSock_Num, pCOAP_RX, size, destip, (uint16_t*)&destport);

#ifdef DEBUG
            printf("Receive: ");
            coap_dump(buf, ret, true);
            printf("\n");
#endif

            if (0 != (ret = coap_parse(&pkt, pCOAP_RX, ret)))
                printf("Bad packet rc=%d\n", ret);
            else
            {
                size_t rsplen = DATA_BUF_SIZE * sizeof(uint8_t);
                coap_packet_t rsppkt;
#ifdef DEBUG
                coap_dumpPacket(&pkt);
#endif
                coap_handle_req(&scratch_buf, &pkt, &rsppkt);

                if (0 != (ret = coap_build(pCOAP_TX, &rsplen, &rsppkt)))
                    printf("coap_build failed rc=%d\n", ret);
                else
                {
#ifdef DEBUG
                    printf("Sending: ");
                    coap_dump(buf, rsplen, true);
                    printf("\n");
#endif
#ifdef DEBUG
                    coap_dumpPacket(&rsppkt);
#endif
                    ret = sendto(COAPSock_Num, pCOAP_TX, rsplen, destip, destport);
                }
            }
         }
         break;
      case SOCK_CLOSED:
         if(socket(COAPSock_Num, Sn_MR_UDP, COAP_SERVER_PORT, 0x00) == COAPSock_Num)
        {
            printf("%d:Opened, UDP loopback, port [%d]\r\n", COAPSock_Num, COAP_SERVER_PORT);
        }
         break;
      default :
         break;
   }
}