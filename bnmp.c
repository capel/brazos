#include "stdlib.h"
#include "sys/ko.h"
#include "inet.h"
#include "mach.h"

#define BUFLEN 4096

#define REQ 1
#define ACK 2

typedef struct pkt {
  uint16_t num;
  uint8_t flags;
  uint8_t remaining;
  char data[];
} __attribute__((packed)) pkt;

typedef struct nmsg {
  inet_pkt* p;
  struct nmsg* next;
} nmsg;

typedef struct ack {
  uint16_t num;
  uint8_t flags;
  uint8_t sent;
} ack;

pkt* mk_pkt(uint16_t num, size_t data_size, char* buf) {
  pkt * p = kmalloc(sizeof(pkt) + data_size);
  p->flags = REQ;
  p->num = num;
  memcpy(p->data, buf, data_size);
  return p;
}

void cleanup_msg(nmsg* m) {
  if (!m) return;
  cleanup_msg(m->next);

  kfree(m->p);
  kfree(m);
}

void print_pkt(const char * verb, pkt* p) {
  printk("%s pkt #%x/%d {%s} %s", verb, p->num, 
      p->remaining, p->flags == REQ ? "REQ" : "ACK", p->data); 
}

nmsg* get_pkt(int sock, inet_addr* from) {
  inet_pkt* p = recv();

  nmsg * m = malloc(sizeof(nmsg));
  m->p = p;
  m->next = 0;

  return m;
}

void nsend(int sock, inet_addr* dst, nmsg* msg) {
  for (nmsg * m = msg; m; m = m->next) {
    print_pkt("sent", m->p);
    int ret = sendto(sock, m->p, m->len, 0, dst, sizeof(inet_addr_in));
    if (ret == -1) perror("send");
  }
}

void send_ack(int sock, inet_addr* dst, uint16_t num, size_t sent) {
  ack a;
  a.num = num;
  a.flags = ACK;
  a.sent = (uint8_t) sent;
  int ret = sendto(sock, (void*)&a, sizeof(a), 0, dst, sizeof(inet_addr_in));
  if (ret == -1) perror("send ack");
}

nmsg* get_msg(int sock, inet_addr* from) {
  nmsg * start = 0;
  nmsg * m = 0;
  for (;;) {
    if (!start) {
      start = get_pkt(sock, from);
      printk("init got\n");
      m = start;
    } else {
      m->next = get_pkt(sock, from);
      printk("second got orig %d m :%d\n",m->p->remaining, m->next->p->remaining);
      m = m->next;
    }

    if (m->p->remaining == 0) {
      printk("bail\n");
      return start;
    }
  }
}
  
void nrecv(int sock, char* buf, size_t len) {
  inet_addr_in from;
  size_t pos = 0;
  size_t num = 0;
  // reorder?
  nmsg * msg = get_msg(sock, (inet_addr*) &from);
  printk("Got msg %p\n", msg);
  for (nmsg * m = msg; m; m = m->next) {
    print_pkt("copied", m->p);
    memcpy(buf + pos, m->p->data, m->len - sizeof(pkt));
    pos += m->len - sizeof(pkt);
    printk("pos %ld\n", pos);
    num++;
  }
  buf[pos] = '\0';
  printk("done\n");

 // send_ack(sock, (inet_addr*)&from, msg->p->num, num);
 // cleanup_msg(msg);
}

int setup_client() {
  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1) perror("socket");

  return s;
}

void setup_dst(inet_addr_in *them) {
  memset(them, 0, sizeof(*them));
  them->sin_family = AF_INET;
  them->sin_port = htons(PORT);
  them->sin_addr.s_addr = inet_addr("127.0.0.1");
}

void client() {
  inet_addr_in them;
  setup_dst(&them);

  int sock = setup_client();

  char* ui = malloc(100);
  size_t size = 100;
  for(;;) {
    getline(&ui, &size, stdin);
    nmsg * m = mk_msg(ui, strlen(ui)+1);
    nsend(sock, (inet_addr*)&them, m);
  }
}

int setup_server() {
  inet_addr_in me, them;
  inet_addr * meptr = (inet_addr*) &me;
  inet_addr * themptr = (inet_addr*) &them;
  size_t them_len = sizeof(them);

  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1) perror("socket");
  
  memset(&me, 0, sizeof(me));
  memset(&them, 0, sizeof(them));
  me.sin_family = AF_INET;
  me.sin_port = htons(PORT);
  me.sin_addr.s_addr = htonl(INADDR_ANY);

  int ret = bind(s, meptr, sizeof(me));
  if (ret == -1) perror("bind");

  return s;
}

void server() {
  int sock = setup_server();
  char buf[BUFLEN];
  for(;;) {
    nrecv(sock, buf, BUFLEN);
    printk(":: %s", buf);
  }
}

