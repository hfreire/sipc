#ifndef __SIP_H__
#define __SIP_H__

#define DEFAULT_PORT 5060
#define MAX_SIP_LEN 1300
#define MAX_SIP_LINES 32
#define DEFAULT_EXPIRATION 86400
/* SIP STATES */
#define INIT 0
#define REGISTERING 1
#define REGISTERED 2
#define INVITING 3
#define INVITED 4
#define ONCALL 5
#define BYE 6
#define UNREGISTERING 7
#define UNREGISTERED 8
#define BYE_OK 9

/* SIP MESSAGES */
#define REGISTER 0
#define INVITE 1
#define INVITE_OK 2
#define ACK 3


#define REQUEST 0
#define ANSWER 1
#define MAX_SDP_LEN 200

#define TRUE 1
#define FALSE 0

typedef struct rtp_session_t {
  int firstpkt;
  int nseqpkt;
  int nseqnon;
} rtp_session_t;

typedef struct sip_call_t {
  char *dip;
  int dport;
  char *dext;
  int sport;
  int dsport;
  char *duser;
  char *id;
  char *tag;
  int socket;
  struct sockaddr_in addr;
  rtp_session_t rtp_session;
} sip_call_t;

typedef struct sip_session_t {
  char *pbxip, *localip, *user;
  int pbxport, localport, ext, curr_state, prev_state, seq, oncall, socket;
  sip_call_t *call;
  struct sockaddr_in addr;
  int power;
} sip_session_t;

typedef struct sip_msg_t {
	char *method;
	char *ip;
	char *version;
	char *via;
	char *maxforwards;
	char *to;
	char *from;
	char *callid;
	int cseq;
	char *contact;
	int expires;
	int contentlength;
	char *contenttype;
	int code;
	int type; /* request = 0 / answer = 1 */
	int rtpport;
	sip_call_t *call;
} sip_msg_t;

extern void init_sip_session(sip_session_t *session);
extern void send_sip_invite(sip_session_t *session);
extern void handle_sip_msg(sip_session_t *session, char *str);
extern void close_sip_session(sip_session_t *session);
extern void send_sip_bye(sip_session_t *session);
extern void free_sip_session(sip_session_t *session);
extern void free_sip_call(sip_call_t *call);

#endif
