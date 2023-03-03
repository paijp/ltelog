/*
	ltelog   https://github.com/paijp/ltelog

	Copyright (c) 2023 paijp

	This software is released under the MIT License.
	http://opensource.org/licenses/mit-license.php
*/

#ifndef	UARTBAUD
#define	UARTBAUD	115200
#endif

#ifndef	BUFFERSIZE
#define	BUFFERSIZE	65536
#endif

#ifndef	SENDINTERVAL
#define	SENDINTERVAL	15000		/* 15s */
#endif

#ifndef	SIMAPN
#define	SIMAPN	"soracom.io"
#endif

#ifndef	SIMUSER
#define	SIMUSER	"sora"
#endif

#ifndef	SIMPASS
#define	SIMPASS	"sora"
#endif

#ifndef	SENDDOMAIN
#define	SENDDOMAIN	"beam.soracom.io"
#endif

#ifndef	SENDPORT
#define	SENDPORT	23080
#endif


#include	<WioLTEforArduino.h>

WioCellular	Wio;

typedef	int	W;
typedef	short	H;
typedef	char	B;
typedef	unsigned int	UW;
typedef	unsigned short	UH;
typedef	unsigned char	UB;


static	byte	buffer[BUFFERSIZE];
static	int	buffer_rpos = 0;
static	int	buffer_wpos = 0;

static	int	socket0 = -1;


static	void	idleproc()
{
	int	c, wpos;
	
	if (!Serial.available())
		return;
	
	c = Serial.read();
	wpos = buffer_wpos + 1;
	if (wpos >= BUFFERSIZE)
		wpos = 0;
	if (wpos == buffer_rpos)
		return;	/* overflow */
	buffer[buffer_wpos] = c;
	buffer_wpos = wpos;
}


void	setup()
{
	delay(200);
	Wio.Init();
	Serial.begin(UARTBAUD);
	Wio.PowerSupplyGrove(true);
	Wio.PowerSupplyLTE(true);
	
	Wio.SetDoWorkInWaitForAvailableFunction(idleproc);
	
	delay(500);
	Wio.LedSetRGB(10, 10, 10);
	delay(500);
	Wio.LedSetRGB(0, 0, 10);
	
	if (!Wio.TurnOnOrReset()) {
		Wio.LedSetRGB(10, 0, 0);
		return;
	}
	Wio.LedSetRGB(0, 10, 10);
	if (!Wio.Activate(SIMAPN, SIMUSER, SIMPASS))
		return;
	Wio.LedSetRGB(0, 10, 0);
	
	for (;;) {
		if ((socket0 = Wio.SocketOpen(SENDDOMAIN, SENDPORT, Wio.SOCKET_UDP)) >= 0)
			break;
		delay(2000);
	}
}


void	loop()
{
	static	byte	sendbuf[1460];
	int	sendbuf_wpos, sendbuf_lfpos;
	int	lfpos;
	
	idleproc();
	if (buffer_rpos == buffer_wpos)
		return;
	
	{
		static	int	remainms = SENDINTERVAL;
		static	int	lastms = -1;
		int	ms;
		
		if (lastms == (ms = millis()))
			return;
		lastms = ms;
		if (--remainms > 0)
			return;
		remainms = SENDINTERVAL;
	}
	
	sendbuf_wpos = sendbuf_lfpos = 0;
	lfpos = 0;
	while (sendbuf_wpos < sizeof(sendbuf)) {
		int	c;
		
		if (buffer_rpos == buffer_wpos)
			break;
		sendbuf[sendbuf_wpos++] = c = buffer[buffer_rpos++];
		if (buffer_rpos >= BUFFERSIZE)
			buffer_rpos = 0;
		if (c == 0xa) {
			sendbuf_lfpos = sendbuf_wpos;
			lfpos = buffer_rpos;
		}
	}
	if (sendbuf_lfpos > 0) {
		sendbuf_wpos = sendbuf_lfpos;
		buffer_rpos = lfpos;
	}
	Wio.LedSetRGB(10, 10, 10);
	Wio.SocketSend(socket0, sendbuf, sendbuf_wpos);
	Wio.LedSetRGB(0, 10, 0);
}

