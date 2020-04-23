/**
 * CIO Get call
 */

#include <atari.h>
#include <6502.h>
#include <stdbool.h>
#include "sio.h"
#include "misc.h"
#include "config.h"

extern unsigned char err;
extern unsigned char ret;
extern unsigned char aux1_save[MAX_DEVICES];
extern unsigned char aux2_save[MAX_DEVICES];
extern unsigned char* rp;
extern unsigned char buffer_rx[MAX_DEVICES][256];
extern unsigned char buffer_rx_len;

extern void _cio_status(void); // Used to get length to fetch.

void _cio_get(void)
{
  unsigned short l;
  err=1;
  if (buffer_rx_len==0)
    {
      // Buffer empty, get length and read it in.
      _cio_status();
      l=(OS.dvstat[1]<<8)+OS.dvstat[0];
      buffer_rx_len=(l<256 ? l : 255);
      
      err=siov(DEVIC_N,
	       OS.ziocb.drive,
	       'R',
	       DSTATS_READ,
	       &buffer_rx,
	       buffer_rx_len,
	       DTIMLO_DEFAULT,
	       buffer_rx_len,
	       0);
      
      rp=&buffer_rx[0];
    }

  if ((buffer_rx_len==0) && (OS.dvstat[2]==0)) // dvstat[2] = disconnected
    {
      err=ret=136;
      return;
    }
  else if (buffer_rx_len==0)
    {
      err=ret=136;
      return;
    }
  
  // Send next char in buffer.
  buffer_rx_len--;
  ret=*rp++;
}
