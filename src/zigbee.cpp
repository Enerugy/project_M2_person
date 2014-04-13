#include <stdio.h>
#include <sys/ioctl.h>
#include "zigbee.hpp"
#include <string.h>
#include <sys/file.h>

//initialiser la port serie
TTY_INFO *readyTTY(int id)
{
    TTY_INFO *ptty;

    ptty = (TTY_INFO *)malloc(sizeof(TTY_INFO));
    if(ptty == NULL)
        return NULL;
    memset(ptty,0,sizeof(TTY_INFO));
    pthread_mutex_init(&ptty->mt,NULL);
    sprintf(ptty->name,"/dev/ttyUSB%d",id);
    //
    // ouvrir la port serie
    ptty->fd = open(ptty->name, O_RDWR | O_NOCTTY |O_NDELAY);
    if (ptty->fd <0)
    {
        free(ptty);
        return NULL;
    }
    //
    // recupere la configuration prescent
    tcgetattr(ptty->fd,&ptty->otm);
    return ptty;
}

///////////////////////////////////////////////////////////////////////////////
// fermer la port serie
int cleanTTY(TTY_INFO *ptty)
{

    if(ptty->fd>0)
    {
        tcsetattr(ptty->fd,TCSANOW,&ptty->otm);
        close(ptty->fd);
        ptty->fd = -1;
        free(ptty);
        ptty = NULL;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// configure la vitesse de port serrie
// ptty struct du tty
// speed vitesse du port serie
// return 0 avic succes
///////////////////////////////////////////////////////////////////////////////
int setTTYSpeed(TTY_INFO *ptty, int speed)
{
    //

    bzero(&ptty->ntm, sizeof(ptty->ntm));
    tcgetattr(ptty->fd,&ptty->ntm);
    ptty->ntm.c_cflag = /*CS8 |*/ CLOCAL | CREAD;

    switch(speed)
    {
    case 300:
        ptty->ntm.c_cflag |= B300;
        break;
    case 1200:
        ptty->ntm.c_cflag |= B1200;
        break;
    case 2400:
        ptty->ntm.c_cflag |= B2400;
        break;
    case 4800:
        ptty->ntm.c_cflag |= B4800;
        break;
    case 9600:
        ptty->ntm.c_cflag |= B9600;
        break;
    case 19200:
        ptty->ntm.c_cflag |= B19200;
        break;
    case 38400:
        ptty->ntm.c_cflag |= B38400;
        break;
    case 57600:
        ptty->ntm.c_cflag |= B57600;
        break;
    case 115200:
        ptty->ntm.c_cflag |= B115200;
        break;
    case 500000:
        ptty->ntm.c_cflag |= B500000;
        break;
    }
    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;
    //
    //
    tcflush(ptty->fd, TCIFLUSH);
    tcsetattr(ptty->fd,TCSANOW,&ptty->ntm);
    //
    //
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
// configuration nb bits de donnee, bit parite, et bit arret
// ptty structure de tty
// databits 7 ou 8 ,nb de bit
// stopbits 1 ou 2 bit arret
// parit N,E,O,,S  bit parite
// return 0 avec success
///////////////////////////////////////////////////////////////////////////////
int setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits)
{
    //
    //
    if( tcgetattr(ptty->fd,&ptty->ntm) != 0)
    {
        printf("SetupSerial [%s]\n",ptty->name);
        return 1;
    }

    bzero(&ptty->ntm, sizeof(ptty->ntm));
    ptty->ntm.c_cflag = CS8 | CLOCAL | CREAD;
    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;
    //
    // configuration port serie
    ptty->ntm.c_cflag &= ~CSIZE;
    switch (databits)
    {  //nb de bits
    case 7:
        ptty->ntm.c_cflag |= CS7;
        break;
    case 8:
        ptty->ntm.c_cflag |= CS8;
        break;
    default:
        printf("Unsupported data size\n");
        return 5;
    }
    //
    //
    switch (parity)
    { // bit parite
    case 'n':
    case 'N':
        ptty->ntm.c_cflag &= ~PARENB; /* Clear parity enable */
        ptty->ntm.c_iflag &= ~INPCK; /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        ptty->ntm.c_cflag |= (PARODD|PARENB);
        ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        ptty->ntm.c_cflag |= PARENB; /* Enable parity */
        ptty->ntm.c_cflag &= ~PARODD;
        ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'S':
    case 's': /*as no parity*/
        ptty->ntm.c_cflag &= ~PARENB;
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    default:
        printf("Unsupported parity\n");
        return 2;
    }
    //
    // bit arret
    switch (stopbits)
    {
    case 1:
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    case 2:
        ptty->ntm.c_cflag |= CSTOPB;
        break;
    default:
        printf("Unsupported stop bits\n");
        return 3;
    }
    //
    //
    ptty->ntm.c_lflag = 0;
    ptty->ntm.c_cc[VTIME] = 0;  // inter-character timer unused
    ptty->ntm.c_cc[VMIN] = 1;  // blocking read until 1 chars received
    tcflush(ptty->fd, TCIFLUSH);
    if (tcsetattr(ptty->fd,TCSANOW,&ptty->ntm) != 0)
    {
        printf("SetupSerial \n");
        return 4;
    }

    return 0;
}

int recvnTTY(TTY_INFO *ptty,char *pbuf,int size)
{
    int ret,left,bytes;

    left = size;

    while(left>0)
    {
        ret = 0;
        bytes = 0;

        pthread_mutex_lock(&ptty->mt);
        ioctl(ptty->fd, FIONREAD, &bytes);
        if(bytes>0)
        {
            ret = read(ptty->fd,pbuf,left);
        }
        pthread_mutex_unlock(&ptty->mt);
        if(ret >0)
        {
            left -= ret;
            pbuf += ret;
        }
        //usleep(100);
    }

    return size - left;
}

int sendnTTY(TTY_INFO *ptty,char *pbuf,int size)
{
    int ret,nleft;
    char *ptmp;

    ret = 0;
    nleft = size;
    ptmp = pbuf;

    while(nleft>0)
    {
        pthread_mutex_lock(&ptty->mt);
        ret = write(ptty->fd,ptmp,nleft);
        pthread_mutex_unlock(&ptty->mt);

        if(ret >0)
        {
            nleft -= ret;
            ptmp += ret;
        }
        //usleep(100);
    }

    return size - nleft;
}
int lockTTY(TTY_INFO *ptty)
{
    if(ptty->fd < 0)
    {
        return 1;
    }

    return flock(ptty->fd,LOCK_EX);
}
int unlockTTY(TTY_INFO *ptty)
{
    if(ptty->fd < 0)
    {
        return 1;
    }

    return flock(ptty->fd,LOCK_UN);
}

