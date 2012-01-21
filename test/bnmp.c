#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define BUFLEN 4096
#define PORT 3333

#define MTU 20

#define REQ 1
#define ACK 2

typedef struct pkt {
  uint16_t num;
  uint8_t flags;
  uint8_t remaining;
  char data[];
} __attribute__((packed)) pkt;

typedef struct nmsg {
  pkt* p;
  size_t len;
  struct nmsg* next;
} nmsg;

typedef struct ack {
  uint16_t num;
  uint8_t flags;
  uint8_t sent;
} ack;

pkt* mk_pkt(uint16_t num, size_t data_size, char* buf) {
  pkt * p = malloc(sizeof(pkt) + data_size);
  p->flags = REQ;
  p->num = num;
  memcpy(p->data, buf, data_size);
  return p;
}

nmsg* mk_msg(char* buf, size_t size) {
  uint16_t num = (uint16_t)rand();
  size_t remaining = size;
  printf("size %ld, size / mtu %ld\n", size, size / MTU);
  size_t num_pkts = 0;
  nmsg* base = NULL;
  nmsg* last = NULL;

  do {
    printf("remaining %ld, pkts %ld\n", remaining, num_pkts);
    nmsg* m = malloc(sizeof(nmsg));
    if (base == NULL) {
      base = m;
      last = m;
    } else {
      last->next = m;
      last = m;
    }
    if (remaining + sizeof(pkt) <= MTU) {
      printf("Fits. Woot!\n");
      m->p = mk_pkt(num, remaining, buf);
      buf += remaining;
      m->len = remaining + sizeof(pkt);
      remaining = 0;
    } else {
      printf("Doesn't fit: MTU %d remaining %ld\n", MTU, remaining);
      m->p = mk_pkt(num, MTU, buf);
      buf += MTU;
      m->len = MTU + sizeof(pkt);
      remaining -= MTU - sizeof(pkt);
    }
    num_pkts++;
  } while (remaining);


  for(nmsg* m = base; m; m = m->next) {
    m->p->remaining = --num_pkts;
  }
  assert(num_pkts == 0);
  return base;
}

void cleanup_msg(nmsg* m) {
  if (!m) return;
  cleanup_msg(m->next);

  free(m->p);
  free(m);
}

void print_pkt(const char * verb, pkt* p) {
  printf("%s pkt #%x/%d {%s} %s\n", verb, p->num, 
      p->remaining, p->flags == REQ ? "REQ" : "ACK", p->data); 
}

nmsg* get_pkt(int sock, struct sockaddr* from) {
  char buf[BUFLEN];
  socklen_t len = sizeof(struct sockaddr_in);

  size_t size = recvfrom(sock, buf, BUFLEN, 0, from, &len);
  pkt *p = malloc(size);
  memcpy(p, buf, size);
  print_pkt("got", p);

  nmsg * m = malloc(sizeof(nmsg));
  m->p = p;
  m->len = size;
  m->next = 0;

  return m;
}

void nsend(int sock, struct sockaddr* dst, nmsg* msg) {
  for (nmsg * m = msg; m; m = m->next) {
    print_pkt("sent", m->p);
    int ret = sendto(sock, m->p, m->len, 0, dst, sizeof(struct sockaddr_in));
    if (ret == -1) perror("send");
  }
}

void send_ack(int sock, struct sockaddr* dst, uint16_t num, size_t sent) {
  ack a;
  a.num = num;
  a.flags = ACK;
  a.sent = (uint8_t) sent;
  int ret = sendto(sock, (void*)&a, sizeof(a), 0, dst, sizeof(struct sockaddr_in));
  if (ret == -1) perror("send ack");
}

nmsg* get_msg(int sock, struct sockaddr* from) {
  nmsg * start = 0;
  nmsg * m = 0;
  for (;;) {
    if (!start) {
      start = get_pkt(sock, from);
      printf("init got\n");
      m = start;
    } else {
      m->next = get_pkt(sock, from);
      printf("second got orig %d m :%d\n",m->p->remaining, m->next->p->remaining);
      m = m->next;
    }

    if (m->p->remaining == 0) {
      printf("bail\n");
      return start;
    }
  }
}
  
void nrecv(int sock, char* buf, size_t len) {
  struct sockaddr_in from;
  size_t pos = 0;
  size_t num = 0;
  // reorder?
  nmsg * msg = get_msg(sock, (struct sockaddr*) &from);
  printf("Got msg %p\n", msg);
  for (nmsg * m = msg; m; m = m->next) {
    print_pkt("copied", m->p);
    memcpy(buf + pos, m->p->data, m->len - sizeof(pkt));
    pos += m->len - sizeof(pkt);
    printf("pos %ld\n", pos);
    num++;
  }
  buf[pos] = '\0';
  printf("done\n");

 // send_ack(sock, (struct sockaddr*)&from, msg->p->num, num);
 // cleanup_msg(msg);
}

int setup_client() {
  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1) perror("socket");

  return s;
}

void setup_dst(struct sockaddr_in *them) {
  memset(them, 0, sizeof(*them));
  them->sin_family = AF_INET;
  them->sin_port = htons(PORT);
  them->sin_addr.s_addr = inet_addr("127.0.0.1");
}

void client() {
  struct sockaddr_in them;
  setup_dst(&them);

  int sock = setup_client();

  char* ui = malloc(100);
  size_t size = 100;
  for(;;) {
    getline(&ui, &size, stdin);
    nmsg * m = mk_msg(ui, strlen(ui)+1);
    nsend(sock, (struct sockaddr*)&them, m);
  }
}

int setup_server() {
  struct sockaddr_in me, them;
  struct sockaddr * meptr = (struct sockaddr*) &me;
  struct sockaddr * themptr = (struct sockaddr*) &them;
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
    printf(":: %s\n", buf);
  }
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("bnmp <client> or bnmp <server>\n");
    exit(1);
  }

  if (!strcmp(argv[1], "client")) {
    client();
  } else if (!strcmp(argv[1], "server")) {
    server();
  } else {
    printf("bnmp <client> or bnmp <server>");
    exit(1);
  }

}
