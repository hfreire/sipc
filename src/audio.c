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
 * @filename: audio.c                                                        *
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include "../include/audio.h"

int audiofd;

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int open_audio()
{
    int arg, err, fd=-1;
    struct stat st;


    /* stat the device */
    err = stat(AUDIODEVICE, &st);
    if (err < 0)
    {
        fprintf(stderr,"cannot stat audio device");
        exit(1);
    }
    /*  verify if is a character device */
    if (!S_ISCHR(st.st_mode))
    {
        fprintf(stderr,"%s is not an audio device\n", AUDIODEVICE);
        exit(1);
    }
    /* try to open the device, otherwise*/
    fd= open(AUDIODEVICE, AUDIOMODE /* | O_NONBLOCK */);
    if ((fd < 0) && (errno == EBUSY))
    {
        printf ("%s is busy\nwaiting...\n", AUDIODEVICE);

        /* Now hang until it's open */
        /* blocking open will wait until other applications stop using it */
        fd= open(AUDIODEVICE, AUDIOMODE);
    }

    if (fd < 0)
    {
        printf("error opening audio device");
        exit(1);
    }

    arg=0x00020008;

    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &arg))
        printf("audio_open: ioctl\n");

    return fd;
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
void close_audio(int fd)
{
    close(fd);
}
