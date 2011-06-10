#ifndef __AUDIO_H__
#define __AUDIO_H__

#define AUDIODEVICE "/dev/audio"
#define BUFSIZE 160
#define AUDIOMODE O_RDWR

extern int open_audio();
extern void close_audio(int fd);

#endif
