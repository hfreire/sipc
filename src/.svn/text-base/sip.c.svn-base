/*****************************************************************************
 *                                                                           *
 *                      10101000100010001010101100101010101                  *
 *                      0101010101               0101010100                  *
 *                      0101010101               0101010100                  *
 *                      1010101010        10101  0101110101                  *
 *                      1010101010       01  10  0101011101                  *
 *                      1010111010       01      0101010101                  *
 *                       010101010  01  010 0101 010101010                   *
 *                       010101010  01  010  01  010101011                   *
 *                        01010101  01  010  01  01010101                    *
 *                        11010101  01  010  01  01010110                    *
 *                         1010101  01  010  01  0101000                     *
 *                         1110101  01  010  01  0101001                     *
 *                          110101      010      010001                      *
 *                           10101     001       01011                       *
 *                            1101  10101        0101                        *
 *                             110               011                         *
 *                              10               01                          *
 *                               01             10                           *
 *                                 00         10                             *
 *                                  01       01                              *
 *                                    01   01                                *
 *                                      010                                  *
 *                                       1                                   *
 *                                                                           *
 *                        Universidade Técnica de Lisboa                     *
 *                                                                           *
 *                          Instituto Superior Técnico                       *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                    RIC - Redes Integradas de Comunicações                 *
 *                               VoIP Application                            *
 *                                                                           *
 *                       Professor Paulo Rogério Pereira                     *
 *                                                                           *
 *                                                                           *
 *****************************************************************************
 * @filename: sip.c                                                          *
 * @description:                                                             *
 * @language: C                                                              *
 * @compiled on: cc/gcc                                                      *
 * @last update at: 2007-11-27                                               *
 *****************************************************************************
 * @students that colaborated on this file:                                  *
 *  57442 - Daniel Silvestre - daniel.silvestre@tagus.ist.utl.pt             *
 *  57476 - Hugo Miguel Pinho Freire - hugo.freire@tagus.ist.utl.pt          *
 *****************************************************************************
 * @changelog for this file:                                                 *
 *	No changelog was kept for this file.                  				           *
 *****************************************************************************
 * @comments for this file:                                                  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "../include/sip.h"

int open_udp_socket(char *ip, int port);
void die_with_error(const char *message);
extern int audiofd;

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
char *mk_sip_msg(sip_session_t *session, int exp, int msgtype)
{
    char *msg, sdp[MAX_SDP_LEN];

    msg = (char *) malloc(MAX_SIP_LEN * sizeof(char));

    switch(msgtype)
    {
    case REGISTER:
        snprintf(msg, MAX_SIP_LEN, \
                 "REGISTER sip:%s:%i SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%i;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: %s <sip:%i@%s:%i>\n"\
                 "From: %s <sip:%i@%s:%i>;tag=456248\n"\
                 "Call-ID: a2e3a@%s\n"\
                 "CSeq: %d REGISTER\n"\
                 "Contact: %s <sip:%i@%s:%i>\n"\
                 "Expires: %i\n"\
                 "Content-Length: 0\n\n\n",\
                 session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->localip,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 exp);
        break;
    case INVITE:
        snprintf(sdp, sizeof(sdp), \
                 "c=IN IP4 %s\n"\
                 "m=audio %d RTP/AVP 0\n", session->localip, session->call->sport);

        snprintf(msg, MAX_SIP_LEN, \
                 "INVITE sip:%s@%s:%d SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: sip:%s@%s:%d\n"\
                 "From: %s <sip:%d@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d INVITE\n"\
                 "Contact: %s <sip:%d@%s:%d>\n"\
                 "Content-Type: application/sdp\n"\
                 "Content-Length: %d\n\n"\
                 "%s",\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 strlen(sdp),\
                 sdp);
        break;
    case INVITE_OK:
        snprintf(sdp, MAX_SDP_LEN, \
                 "c=IN IP4 %s\n"\
                 "m=audio %d RTP/AVP 0\n", session->localip, session->call->sport);

        snprintf(msg, MAX_SIP_LEN, \
                 "SIP/2.0 200 OK\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "To: sip:%d@%s:%d;tag=%s\n"\
                 "From: %s\" <sip:%s@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d INVITE\n"\
                 "Contact: %s <sip:%d@%s:%d>\n"\
                 "Content-Type: application/sdp\n"\
                 "Content-Length: %d\n"\
                 "\n"\
                 "%s",
                 session->call->dip, session->call->dsport,\
                 session->ext, session->pbxip, session->pbxport, session->call->tag,\
                 session->call->duser, session->call->dext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 strlen(sdp),\
                 sdp);
        break;
    case ACK:
        snprintf(msg, MAX_SIP_LEN, \
                 "ACK sip:%s@%s:%d SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: sip:%s@%s:%d;tag=%s\n"\
                 "From: %s <sip:%i@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d ACK\n"\
                 "Contact: %s <sip:%i@%s:%d>\n"\
                 "Content-Length: 0\n\n\n",\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->call->dext, session->pbxip, session->pbxport, session->call->tag,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport);
        break;
    case BYE:
        snprintf(msg, MAX_SIP_LEN, \
                 "BYE sip:%s@%s:%d SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "From: sip:%i@%s:%d;tag=%s\n"\
                 "To: sip:%s@%s:%d;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d BYE\n"\
                 "Content-Length: 0\n\n\n",
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->ext, session->pbxip, session->pbxport, session->call->tag,\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq);
        break;
    case BYE_OK:
        snprintf(msg, MAX_SIP_LEN, \
                 "SIP/2.0 200 OK\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "From: sip:%s@%s:%d;tag=%s\n"\
                 "To: sip:%i@%s:%d;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d BYE\n"\
                 "Content-Length: 0\n\n\n",
                 session->call->dip, session->call->dport,\
                 session->call->dext, session->pbxip, session->pbxport, session->call->tag,\
                 session->ext,  session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq);
        break;
    }

    return msg;
}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
char *gen_call_id(sip_session_t *session)
{
    char *s;

    s = (char *) malloc(10*sizeof(char));
    snprintf(s, 9, "%i", rand());

    return s;
}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
sip_msg_t *read_sip_msg(char *str)
{
    char **s, *t, *r, n[3][100];
    int i, k, p= 0;
    sip_msg_t *msg;

    s = (char **) malloc(MAX_SIP_LINES * sizeof(char*));
    t = str;

    for(i=0; (r = strchr(t, '\n')) != NULL;i++)
    {
        *r = '\0';
        s[i] = (char *) malloc((strlen(t)+1) * sizeof(char));
        strcpy(s[i], t);
        *r = '\n';
        t = r+1;
    }

    s[i]=NULL;

    msg = (sip_msg_t *) malloc(sizeof(sip_msg_t));
    msg->call = (sip_call_t *) malloc(sizeof(sip_call_t));
    msg->call->dip = NULL;
    msg->call->dext = NULL;
    msg->call->duser = NULL;
    msg->call->tag = NULL;
    msg->call->id = NULL;
    msg->method = NULL;
    msg->ip = NULL;
    msg->version = NULL;
    msg->via = NULL;
    msg->maxforwards = NULL;
    msg->to = NULL;
    msg->from = NULL;
    msg->callid = NULL;
    msg->contact = NULL;
    msg->contenttype = NULL;

    printf("%s\n",s[0]);

    sscanf(s[0], "%s %s %s", n[0], n[1], n[2]);

    if(strstr(n[0], "SIP"))
    {
        msg->type = ANSWER;
        msg->code = atoi(n[1]);
    }
    else if(strstr(n[2], "SIP"))
    {
        msg->type = REQUEST;
        msg->method = (char *) malloc(strlen(n[0])*sizeof(char));
        memcpy(msg->method,n[0],strlen(n[0]));
    }

    for(k=0;k<i;k++)
    {

        t = strchr(s[k],':');
        if(t != NULL)
        {
            *t = '\0';

            if(!strcmp(s[k],"Via"))
            {
                msg->via = (char *) malloc(strlen(t+2)*sizeof(char));
                memcpy(msg->via,t+2,strlen(t+2));
            }
            else if(!strcmp(s[k],"To"))
            {
                msg->to = (char *) malloc(strlen(t+2)*sizeof(char));
                memcpy(msg->to,t+2,strlen(t+2));
                r = strchr(t+2,'=');
                if(r != NULL)
                {
                    r++;
                    msg->call->tag = (char *) malloc((strlen(r)+1)*sizeof(char));
                    memset(msg->call->tag,'\0',strlen(r)+1);
                    memcpy(msg->call->tag,r,strlen(r)+1);
                }
            }
            else if(!strcmp(s[k],"From"))
            {
                msg->from = (char *) malloc(strlen(t+2)*sizeof(char));
                memcpy(msg->from,t+2,strlen(t+2));

                if((r = strchr(t+2,' ')) != NULL)
                {
                    *r = '\0';
                    msg->call->duser = (char *) malloc((strlen(t+2)+1)*sizeof(char));
                    memset(msg->call->duser,'\0',strlen(t+2)+1);
                    memcpy(msg->call->duser,t+2,strlen(t+2)+1);
                }
                else if((r = strchr(t+2,'"')) != NULL)
                {

                    *r = '\0';
                    r = strchr(t+3,'"');
                    *r = '\0';
                    msg->call->duser = (char *) malloc((strlen(t+3)+1)*sizeof(char));
                    memset(msg->call->duser,'\0',strlen(t+3)+1);
                    memcpy(msg->call->duser,t+3,strlen(t+3)+1);

                }
            }
            else if(!strcmp(s[k],"Call-ID"))
            {
                msg->callid = (char *) malloc((strlen(t+2)+1)*sizeof(char));
                memset(msg->callid,'\0',strlen(t+2)+1);
                memcpy(msg->callid,t+2,strlen(t+2)+1);

                msg->call->id = (char *) malloc((strlen(t+2)+1)*sizeof(char));
                memset(msg->call->id,'\0',strlen(t+2)+1);
                memcpy(msg->call->id,t+2,strlen(t+2)+1);
            }
            else if(!strcmp(s[k],"CSeq"))
            {
                msg->cseq = atoi(t+2);
            }
            else if(!strcmp(s[k],"Contact"))
            {
                msg->contact = (char *) malloc((strlen(t+2)+1)*sizeof(char));
                memset(msg->contact,'\0',strlen(t+2)+1);
                memcpy(msg->contact,t+2,strlen(t+2)+1);
                r = strchr(t+2,':');
                r++;
                t =strchr(r,'@');
                *t = '\0';
                msg->call->dext = (char *) malloc((strlen(r)+1)*sizeof(char));
                memset(msg->call->dext,'\0',strlen(r)+1);
                memcpy(msg->call->dext,r,strlen(r)+1);
                t++;
                r = strchr(t,':');
                if(r == NULL)
                {
                    msg->call->dsport = DEFAULT_PORT;
                    r = strchr(t,'>');
                }
                *r = '\0';
                msg->call->dip = (char *) malloc((strlen(t)+1)*sizeof(char));
                memset(msg->call->dip,'\0',strlen(r)+1);
                memcpy(msg->call->dip,t,strlen(t)+1);
                if(msg->call->dsport != DEFAULT_PORT)
                {
                    r++;
                    t = strchr(r,'>');
                    *t = '\0';
                    msg->call->dsport = atoi(r);
                }

            }
            else if(!strcmp(s[k],"Content-Type"))
            {

                r = strchr(t+2,'/');
                *r = '\0';
                if(!strcmp(t+2,"application"))
                {

                    p = 1;
                }

            }
            else if(!strcmp(s[k],"Content-Length"))
            {
                msg->contentlength = atoi(t+2);

            }
            else if(!strcmp(s[k],"Max-Forwards"))
            {
                msg->maxforwards = (char *) malloc(strlen(t+2)*sizeof(char));
                memcpy(msg->maxforwards,t+2,strlen(t+2));
            }
            else if(!strcmp(s[k],"Expires"))
            {
                msg->expires = atoi(t+2);
            }
        }


        if(p==1)
        {

            r = strchr(s[k],' ');
            if (r == NULL) continue;
            *r = '\0';

            if(!strcmp(s[k],"m=audio"))
            {
                msg->call->dport = atoi(r+1);
            }
        }
    }

    for(k=0;k<i+1;k++) free(s[k]);
    free(s);

    return msg;

}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void free_sip_msg(sip_msg_t *msg)
{

    if(msg->method != NULL) free(msg->method);
    if(msg->ip != NULL) free(msg->ip);
    if(msg->maxforwards != NULL) free(msg->maxforwards);
    if(msg->to != NULL) free(msg->to);
    if(msg->from != NULL) free(msg->from);
    if(msg->callid != NULL) free(msg->callid);
    if(msg->contact != NULL) free(msg->contact);
    if(msg->contenttype != NULL) free(msg->contenttype);

    free(msg);

}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void free_sip_call(sip_call_t *call)
{
    if(call->dext != NULL) free(call->dext);
    if(call->dip != NULL) free(call->dip);
    if(call->duser != NULL) free(call->duser);
    if(call->tag != NULL) free(call->tag);
    if(call->id != NULL) free(call->id);

    free(call);
}

/*********************************************Alice 4001@192.168.0.6:5060 192.168.0.100:5070********************************
 * @name
 * @description
 * @param						
 * @return
 */
void free_sip_session(sip_session_t *session)
{

    if(session->pbxip != NULL) free(session->pbxip);
    if(session->localip != NULL) free(session->localip);
    if(session->user != NULL) free(session->user);

    free(session);
}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void send_sip_register(sip_session_t *session, int exp)
{
    int n;
    char *msg;

    msg = mk_sip_msg(session, exp, REGISTER);

    if ((n = sendto(session->socket, msg, strlen(msg) + 1, 0, (struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(msg) + 1)
        die_with_error("sendto() failed");

    free(msg);

}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void send_sip_invite(sip_session_t *session)
{
    int l;
    char *s;
    struct sockaddr_in addr;

    s = gen_call_id(session);
    session->call->id = (char *) malloc(strlen(s)*sizeof(char));
    memcpy(session->call->id,s,strlen(s));

    session->call->socket = open_udp_socket(session->localip,0);

    l = sizeof(addr);
    if(getsockname(session->call->socket,(struct sockaddr *) &addr,(socklen_t *) &l))
        die_with_error("getsockname() failed");

    session->call->sport = ntohs(addr.sin_port);

    s = mk_sip_msg(session,0,INVITE);

    if((l =sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr)) ) != strlen(s)+1)
        die_with_error("sendto() failed");

    session->prev_state = REGISTERED;
    session->curr_state = INVITING;

}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void send_sip_bye(sip_session_t *session)
{
    int n;
    char *s;

    s = mk_sip_msg(session,0,BYE);

    if((n =sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr)) ) != strlen(s)+1)
        die_with_error("sendto() failed");

    close(session->call->socket);

    session->prev_state = ONCALL;
    session->curr_state = BYE;
}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void init_sip_session(sip_session_t *session)
{

    session->call = NULL;
    session->seq = 1;
    session->curr_state = REGISTERING;
    session->prev_state = INIT;

    session->socket = open_udp_socket(session->localip, session->localport);

    send_sip_register(session, DEFAULT_EXPIRATION);

}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void close_sip_session(sip_session_t *session)
{
    send_sip_register(session, 0);

    session->prev_state = REGISTERED;
    session->curr_state = UNREGISTERING;
}

/*****************************************************************************
 * @name
 * @description
 * @param						
 * @return
 */
void handle_sip_msg(sip_session_t *session, char *str)
{
    int l;
    char *s;
    sip_msg_t *msg;
    struct sockaddr_in addr;

    msg = read_sip_msg(str);

    switch(session->curr_state)
    {
    case REGISTERING:
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {
            fprintf(stdout,"Registo efectuado com sucesso.\n");
            session->prev_state = REGISTERING;
            session->curr_state = REGISTERED;
        }
        else if(msg->type == ANSWER && msg->code < 100 && msg->code > 200)
        {
            fprintf(stdout,"Registo não efectuado.\n");
            exit(1);
        }
        free_sip_call(msg->call);
        break;
    case REGISTERED:
        if(msg->type == REQUEST && !strcmp(msg->method,"INVITE") && msg->contact != NULL)
        {
            session->call = msg->call;
            session->seq = msg->cseq;

            session->call->sport = ntohs(addr.sin_port);


            session->call->addr.sin_family = AF_INET;
            session->call->addr.sin_addr.s_addr = inet_addr(session->call->dip);
            session->call->addr.sin_port = htons(session->call->dport);
            session->call->rtp_session.nseqpkt = 0;
            session->call->rtp_session.firstpkt = TRUE;


            session->call->socket = open_udp_socket(session->localip,session->call->sport);

            l = sizeof(addr);
            //if(getsockname(session->call->socket, (struct sockaddr *) &addr, (socklen_t *) &l)) die_with_error("getsockname() //failed");



            s = mk_sip_msg(session, DEFAULT_EXPIRATION, INVITE_OK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1)
                die_with_error("sendto() failed");

            session->prev_state = REGISTERED;
            session->curr_state = INVITED;

        }
        break;
    case INVITING:
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {
            printf("ACK\n");

            session->call->dport = msg->call->dport;

            session->call->tag = (char *) malloc((strlen(msg->call->tag)+1)*sizeof(char));
            memset(session->call->tag,'\0',strlen(msg->call->tag)+1);
            memcpy(session->call->tag,msg->call->tag,strlen(msg->call->tag));
            session->call->addr.sin_family = AF_INET;
            session->call->addr.sin_addr.s_addr = inet_addr(session->call->dip);
            session->call->addr.sin_port = htons(session->call->dport);



            s = mk_sip_msg(session, DEFAULT_EXPIRATION, ACK);



            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1)
                die_with_error("sendto() failed");

            free_sip_call(msg->call);

            session->prev_state = INVITING;
            session->curr_state = ONCALL;

        }
        else if(msg->type == ANSWER && msg->code == 404)
        {

            session->call->tag = (char *) malloc((strlen(msg->call->tag)+1)*sizeof(char));
            memset(session->call->tag,'\0',strlen(msg->call->tag)+1);
            memcpy(session->call->tag,msg->call->tag,strlen(msg->call->tag));
            //session->call->addr.sin_family = AF_INET;
            //session->call->addr.sin_addr.s_addr = inet_addr(session->call->dip);
            //session->call->addr.sin_port = htons(session->call->dport);

            s = mk_sip_msg(session, DEFAULT_EXPIRATION, ACK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1)
                die_with_error("sendto() failed");

            free_sip_call(msg->call);
            free_sip_call(session->call);
            session->call = NULL;

            fprintf(stdout,"\nNão foi encontrada nenhuma aplicação VoIP no destino.\n");
            session->prev_state = INVITING;
            session->curr_state = REGISTERED;
        }
        break;
    case INVITED:
        if(msg->type == REQUEST && !strcmp(msg->method,"ACK"))
        {

            session->prev_state = INVITED;
            session->curr_state = ONCALL;
        }
        free_sip_call(msg->call);
        break;
    case ONCALL:
        if(msg->type == REQUEST && !strcmp(msg->method,"BYE"))
        {
            session->seq = msg->cseq;

            s = mk_sip_msg(session, DEFAULT_EXPIRATION, BYE_OK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1)
                die_with_error("sendto() failed");

            close(session->call->socket);
            free_sip_call(session->call);
            session->call = NULL;
            free_sip_call(msg->call);

            session->prev_state = ONCALL;
            session->curr_state = REGISTERED;
        }
        break;
    case BYE:
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {

            session->prev_state = BYE;
            session->curr_state = REGISTERED;
            free_sip_call(msg->call);
            free_sip_call(session->call);
            session->call = NULL;
        }
        break;
    case UNREGISTERING:
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {
            session->prev_state = REGISTERED;
            session->curr_state = UNREGISTERED;
            free_sip_call(msg->call);
        }
        break;

    }

    free_sip_msg(msg);

}
