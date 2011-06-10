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
 *                               VOIP Application                            *
 *                                                                           *
 *                       Professor Paulo Rogério Pereira                     *
 *                                                                           *
 *                                                                           *
 *****************************************************************************
 * @filename: voz.c                                                          *
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
 *	No changelog was kept for this file.                  									 *
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
#include "../include/audio.h"
#include "../include/sip.h"
#include "../include/rtp.h"
#include "../include/g711.h"

extern int audiofd;

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
void usage()
{
    fprintf(stderr,"Usage: voz user extension@pbx_ip[:port] local_ip[:port]\n");
    exit(1);
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
void die_with_error(const char *message)
{
    perror(message);
    exit(1);
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int open_udp_socket(char *ip, int port)
{
    int s, i;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port > 0 ? port:INADDR_ANY);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        die_with_error("socket() failed");

    if ((i = bind(s, (struct sockaddr *) &addr, sizeof(addr))) < 0)
        die_with_error("bind() failed");

    return s;
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int get_power(char *buf)
{
    int a, i, p;

    for (a=0, i=0, p=0 ; i < BUFSIZE ; i++ )

        a = a + (st_ulaw2linear16(buf[i])*st_ulaw2linear16(buf[i]));

    p = a/BUFSIZE;

    return p;
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int main(int argc, char **argv)
{
    char *r, *t, msg[MAX_SIP_LEN], c, buf[BUFSIZE];
    struct sockaddr_in from;
    int n, p;
    socklen_t fromlen;
    sip_session_t *session;
    fd_set mask;

    session = (sip_session_t *) malloc(sizeof(struct sip_session_t));
    session->pbxport = session->localport = -1;
    fromlen = sizeof(from);

    /* verify syntax */
    if (argc != 4 && argc != 5) usage();

    /* parse arguments */
    session->user = (char *) malloc((strlen(argv[1])+1)*sizeof(char));
    memcpy(session->user, argv[1], strlen(argv[1])+1);
    r = strchr(argv[2], '@');
    if (r == NULL) usage();
    *r = '\0';
    session->ext = atoi(argv[2]);
    r++;
    t = strchr(r, ':');
    if (t == NULL) session->pbxport = DEFAULT_PORT;
    else *t = '\0';
    session->pbxip = (char *) malloc((strlen(r)+1)*sizeof(char));
    memcpy(session->pbxip, r, strlen(r)+1);
    if (session->pbxport < 0)
    {
        t++;
        session->pbxport = atoi(t);
    }
    r = strchr(argv[3], ':');
    if (r == NULL) session->localport = DEFAULT_PORT;
    else *r = '\0';
    session->localip = (char *) malloc((strlen(argv[3])+1)*sizeof(char));
    memcpy(session->localip, argv[3], strlen(argv[3])+1);
    if (session->localport < 0)
    {
        r++;
        session->localport = atoi(r);
    }
    if(argc == 5) session->power = atoi(argv[4]);

    /* open audio device */
    audiofd = open_audio();

    session->addr.sin_family = AF_INET;
    session->addr.sin_addr.s_addr = inet_addr(session->pbxip);
    session->addr.sin_port = htons(session->pbxport);

    init_sip_session(session);

    while (1)
    {


        if ( session->prev_state != session->curr_state )
        {
            switch (session->curr_state)
            {
            case REGISTERED:
                fprintf(stdout, "\nL - Ligar a outra aplicação VoIP\nT - Terminar aplicação\n\n");
                break;
            case ONCALL:
                fprintf(stdout, "\nD - Desligar a chamada de voz\n\n");
                break;
            case INVITING:
                fprintf(stdout, "\nD - Desligar a chamada de voz\n\n");
                break;
            case UNREGISTERED:
                close(session->socket);

                if(session->call != NULL)
                {

                    free_sip_call(session->call);
                }
                free_sip_session(session);
                close(audiofd);

                exit(0);
            }

            session->prev_state = session->curr_state;

        }

        FD_ZERO(&mask);
        FD_SET(session->socket, &mask);
        FD_SET(fileno(stdin), &mask);

        if (session->curr_state == ONCALL)
        {
            FD_SET(session->call->socket, &mask);
            FD_SET(audiofd, &mask);
        }

        select(FD_SETSIZE, &mask, 0, 0, 0);


        if (session->curr_state == ONCALL)
        {

            if(FD_ISSET(session->call->socket, &mask))
            {

                p = rtp_recvfrom(session->call->socket, buf, BUFSIZE);

                if ((n = write(audiofd, buf, p)) < 0)
                    die_with_error("write() failed");
            }

            if (FD_ISSET(audiofd, &mask))
            {
                session->call->rtp_session.nseqnon++;

                if ((n = read(audiofd, buf, BUFSIZE )) < 0)
                    die_with_error("read() failed");

                //     if ((n = write(audiofd, buf, BUFSIZE)) < 0)
                //         die_with_error("write() failed");


                if(session->power < get_power(buf))
                {
                    printf("%i\n",get_power(buf));
                    session->call->rtp_session.nseqpkt++;
                    rtp_sendto(session, buf, n, &(session->call->addr));
                }
                else session->call->rtp_session.firstpkt = TRUE;

            }
        }


        /* handle sip messages from pbx */
        if (FD_ISSET(session->socket, &mask))
        {

            memset(msg,0,MAX_SIP_LEN);
            if ((n = recvfrom(session->socket,msg,MAX_SIP_LEN,0,(struct sockaddr *) &from, &fromlen)) < 0)
                die_with_error("recvfrom() failed");

            handle_sip_msg(session,msg);

        }

        if (FD_ISSET(fileno(stdin), &mask))
        {
            c = getchar();

            switch (c)
            {
            case 'L':

                if (session->curr_state == REGISTERED)
                {
                    fprintf(stdout,"\nExtensão: ");
                    fscanf(stdin,"%s",t);
                    fprintf(stdout,"IP: ");
                    fscanf(stdin,"%s",r);
                    fprintf(stdout,"Porto: ");
                    fscanf(stdin,"%i",&p);

                    session->call = (sip_call_t *) malloc(sizeof(sip_call_t));
                    session->call->dext = (char *) malloc((strlen(t)+1)*sizeof(char));
                    memset(session->call->dext,'\0',strlen(t)+1);
                    memcpy(session->call->dext,t,strlen(t));
                    session->call->dip = (char *) malloc((strlen(r)+1)*sizeof(char));
                    memcpy(session->call->dip,r,strlen(r)+1);
                    session->call->dport = p;
                    session->call->duser = NULL;
                    session->call->id = NULL;
                    session->call->tag = NULL;
                    session->call->rtp_session.nseqpkt = 0;
                    session->call->rtp_session.nseqnon = 0;
                    session->call->rtp_session.firstpkt = TRUE;

                    send_sip_invite(session);

                }
                break;
            case 'D':
                if (session->curr_state == ONCALL)
                    send_sip_bye(session);
                else if (session->curr_state == INVITING)
                    printf("Estou aqui e enviei um cancel\n");
                break;
            case 'T':
                if (session->curr_state == REGISTERED)
                    close_sip_session(session);
                break;


            }
        }







    }

    return 0;
}
