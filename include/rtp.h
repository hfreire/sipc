#include "sip.h"

#define RTP_HEADER_LEN 12

#define VPXCCMPT_FIXO 0x8000

#define VPXCCMPT_SHIFT 16
#define VPXCCMPT_MASK 0xFFFF0000
#define SEQ_MASK 0x0000FFFF

#define SSRC 154332

#define TRUE 1
#define FALSE 0

extern int rtp_recvfrom(int s, char *buf, int bufsize);
extern int rtp_sendto(sip_session_t *session, char *buf, int bufsize, struct sockaddr_in *addr);
