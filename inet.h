#ifndef INET_H
#define INET_H

#include "types.h"

#define MTU 1000

typedef unsigned uint32_t;
typedef unsigned short uint16_t;

typedef struct inet_addr {
  uint32_t ip;
  uint16_t port;
} inet_addr;

typedef struct inet_pkt {
  inet_addr addr;
  size_t len;
  char* buf;
} inet_pkt;

inet_pkt* inet_recv(void);
void inet_send(inet_pkt* pkt);

#endif
