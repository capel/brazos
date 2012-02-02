#include "inet.h"
#include "stdlib.h"
#include "kio.h"
#include "mem.h"
#include "sys/ko.h"

#define DEV_ETHER_ADDRESS 0x14000000

#define P(x) ((void*)(x))

char * dev_buf = P(DEV_ETHER_ADDRESS + 0x0000);
#define	    DEV_BUFFER_SIZE	    0x4000

volatile int* dev_status = P(DEV_ETHER_ADDRESS + 0x4000);
volatile int* dev_cmd = P(DEV_ETHER_ADDRESS + 0x4020);

int* dev_pkt_len = P(DEV_ETHER_ADDRESS + 0x4010);
uint32_t* dev_addr = P(DEV_ETHER_ADDRESS + 0x4040);
uint16_t* dev_port = P(DEV_ETHER_ADDRESS + 0x4060);

/*  Status bits:  */
#define PKT_READY 2

/*  Commands:  */
#define	CMD_ACK		0
#define	CMD_TX		1

unsigned short swap16(unsigned short i);
unsigned int swap32(unsigned i);

inet_pkt* inet_recv(void) {
  if (*dev_status != PKT_READY) {
    return 0;
  }

  inet_pkt *pkt = kmalloc(sizeof(inet_pkt));
  pkt->addr.ip = swap32(*dev_addr);
  pkt->addr.port = swap16(*dev_port);

  pkt->len = *dev_pkt_len;
  pkt->buf = kmalloc(pkt->len);
  memcpy(pkt->buf, dev_buf, pkt->len);

  *dev_cmd = CMD_ACK;
  return pkt;
}

void inet_send(inet_pkt* pkt) {
  if (dev_status != 0) {
    panic("Inet Tx when a packet in pending (eg, during interrupt?!)");
  }

  *dev_addr = swap32(pkt->addr.ip);
  *dev_port = swap16(pkt->addr.port);

  assert(pkt->len < MTU);
  *dev_pkt_len = pkt->len;

  memcpy(dev_buf, pkt->buf, pkt->len);
  *dev_cmd = CMD_TX;
}
