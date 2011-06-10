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
 * @filename: rtp.c                                                          *
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
#include "../include/rtp.h"
#include "../include/sip.h"


void die_with_error(const char *message);

char *mk_rtp_header(sip_session_t *session, int n)
{
    char *rtp_header;

    int  timestamp;

    rtp_header = (char *) malloc(RTP_HEADER_LEN*sizeof(char));
    memset(rtp_header,0,RTP_HEADER_LEN);

    rtp_header[0] = 0x80;

    if(session->call->rtp_session.firstpkt == TRUE)
    {
        rtp_header[1] = 0x80;
        session->call->rtp_session.firstpkt = FALSE;
    }
    else
    {
        rtp_header[1] = 0x00;
    }

    rtp_header[3] = session->call->rtp_session.nseqpkt&0xFF;
    rtp_header[2] = (session->call->rtp_session.nseqpkt&0xFF00)>>8;

    timestamp = n*(session->call->rtp_session.nseqnon-1);
    rtp_header[7] = timestamp&0xFF;
    rtp_header[6] = (timestamp&0xFF00)>>8;
    rtp_header[5] = (timestamp&0xFF0000)>>16;
    rtp_header[4] = (timestamp&0xFF000000)>>24;

    /* SSRC */
    rtp_header[11] = SSRC&0xFF;

    return rtp_header;
}

int rtp_recvfrom(int s, char *buf, int bufsize)
{

    char tmpbuf[bufsize+RTP_HEADER_LEN];
    struct sockaddr_in from;
    int header[4], n, fromlen;

    fromlen = sizeof(from);

    if((n = recvfrom(s, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) &from, (socklen_t *) &fromlen)) != bufsize+RTP_HEADER_LEN)
        die_with_error("recvfrom() failed");

    memcpy(header, tmpbuf, RTP_HEADER_LEN);
    memcpy(buf, tmpbuf+RTP_HEADER_LEN, n - RTP_HEADER_LEN);

    return n-RTP_HEADER_LEN;
}

int rtp_sendto(sip_session_t *session, char *buf, int bufsize, struct sockaddr_in *addr)
{

    int n;
    char tmpbuf[bufsize+RTP_HEADER_LEN];
    char *rtp_header;

    rtp_header = mk_rtp_header(session,bufsize);

    memcpy(tmpbuf, rtp_header, RTP_HEADER_LEN);
    memcpy(tmpbuf+RTP_HEADER_LEN, buf, bufsize);

    if((n = sendto(session->call->socket, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) addr, sizeof(struct sockaddr_in))) != bufsize+RTP_HEADER_LEN)
        die_with_error("sendto() failed");

    return n-RTP_HEADER_LEN;

}
