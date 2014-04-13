#ifndef __ZIGBEE_H__
#define __ZIGBEE_H__

/**
    @file zigbee.hpp
    @author ise2
    @version 1.0
    @date 8 mars 2014
 
    @brief Fonctions permettant de configurer une interface usb ainsi que d'envoyer et recevoir des données via zigbee
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>

//
// stucture du port serie
typedef struct tty_info_t
{
    int fd;                // id du port serie
    pthread_mutex_t mt;    // mutex
    char name[24];        // nom du port serie
    struct termios ntm;    // nouveau configuration
    struct termios otm;    // ancien configuration
} TTY_INFO;


TTY_INFO *readyTTY(int id);
int setTTYSpeed(TTY_INFO *ptty, int speed);
int setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits);
int cleanTTY(TTY_INFO *ptty);
int sendnTTY(TTY_INFO *ptty,char *pbuf,int size);
int recvnTTY(TTY_INFO *ptty,char *pbuf,int size);
int lockTTY(TTY_INFO *ptty);
int unlockTTY(TTY_INFO *ptty);

#endif
