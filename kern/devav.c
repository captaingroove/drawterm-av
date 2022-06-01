#include	"u.h"
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"devav.h"
#include	"draw.h"

/// Problem:
/// Get an Image to write video frames to when the user process opens /dev/av
///
/// Ideas:
/// 1. Send an rcpu message to rio which runs on the cpu server to open a window and get information
///    about this window, somehow.
/// 2. Create a window using /dev/draw, drawterm should know the details about all current windows
///    on the display / screen / _screen. The global variable display is declared in draw.h.
///    ... but not defined anywhere ... linker error.
/// 3. Is there a way that devices communicate with each other?
/// 4. Do s.th. similar like in kern/devdraw.c:
///    - gets 9P draw messages and calls memdraw functions (fast local layer for graphics)
///    - Client* passed to devdraw.c:1410 function drawmesg(Client *client, void *av, int n)
///    - drawmesg() is called by devdraw.c:1259 function drawwrite(Chan *c, void *a, long n, vlong off)
///      cl = drawclient(c)
///    ... see below for code snippets from devdraw.c ...
///    - device hierarchy like with /dev/draw:
///    /dev/av/new
///    /dev/av/n/data
///    /dev/av/n/ctl
/// 5. Extend devdraw.c to handle AV streams <-- this might be the way to go forward
///    - add devices to /dev/draw
///    /dev/draw/n/avdata
///    /dev/draw/n/avctl
///    - windows created can be handled like any other window on client side
///    - draw video frames using libmemdraw
///    - stream audio directly to the host os audio layer
///
///////////////////////////////////////////////////////////////////////////////////////////////////
///
///    Code snippets from devdraw.c:
///
///      Client*
///      drawclient(Chan *c)
///      {
///      	Client *client;
///
///      	client = drawclientofpath(c->qid.path);
///      	if(client == nil)
///      		error(Enoclient);
///      	return client;
///      }
///

///      Client*
///      drawclientofpath(ulong path)
///      {
///      	Client *cl;
///      	int slot;
///
///      	slot = CLIENTPATH(path);
///      	if(slot == 0)
///      		return nil;
///      	cl = sdraw.client[slot-1];
///      	if(cl==0 || cl->clientid==0)
///      		return nil;
///      	return cl;
///      }

///      static	Draw		sdraw;
///      with
///        Client* drawnewclient(void)
///      allocating the client (for the sdraw.client list) when called in:
///        static Chan* drawopen(Chan *c, int omode)
///      and cl is also returned to the client in drawopen():
///    	   if(QID(c->qid) == Qnew){
///    	   	 cl = drawnewclient();
///    	   	 if(cl == 0)
///    	   	 	error(Enodev);
///    	   	 c->qid.path = Qctl|((cl->slot+1)<<QSHIFT);
///    	   }

///		 drawmesg(cl, a, n);

///      devdraw.c:469 function drawlookup() returns the Image of the client
///
///      with imageId id
///      DImage*
///      drawlookup(Client *client, int id, int checkname)
///      { ... }
///
///////////////////////////////////////////////////////////////////////////////////////////////////


enum
{
	Qdir		= 0,
	Qav,

	Vclosed		= 0,
	Vread,
	Vwrite,
};

Dirtab
avdir[] =
{
	".",	{Qdir, 0, QTDIR},		0,	DMDIR|0555,
	"av",	{Qav},		0,	0666,
};

static	struct
{
	QLock	lk;
	Rendez	vous;
	int	amode;		/* Vclosed/Vread/Vwrite for /av */
} av;

#define aqlock(a) qlock(&(a)->lk)
#define aqunlock(a) qunlock(&(a)->lk)

static	char	Emode[]		= "illegal open mode";
static Image *back, *dline;
static Image *av_screen;
static Screen *_av_screen;


static void
avinit(void)
{
}

static Chan*
avattach(char *param)
{
	return devattach('V', param);
}

static Walkqid*
avwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, avdir, nelem(avdir), devgen);
}

static int
avstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, avdir, nelem(avdir), devgen);
}

static Chan*
avopen(Chan *c, int omode)
{
	int amode;

	switch((ulong)c->qid.path) {
	default:
		error(Eperm);
		break;

	case Qdir:
		break;

	case Qav:
		amode = Vwrite;
		if((omode&7) == OREAD)
			amode = Vread;
		aqlock(&av);
		if(waserror()){
			aqunlock(&av);
			nexterror();
		}
		if(av.amode != Vclosed)
			error(Einuse);

		/* avdevopen(); */

		/* if (initdraw(0, 0, "clock") < 0) */
		/* 	sysfatal("initdraw failed"); */
		/* back = allocimagemix(display, DPalebluegreen, DWhite); */
		/* dline = allocimage(display, Rect(0,0,1,1), CMAP8, 1, DPaleblue); */

		/// Create a rio window for the video display using the rio files (see rio(4))
		/* srvwsys = getenv("wsys"); */
		/* if(srvwsys == nil) */
		/* 	sysfatal("can't find $wsys: %r"); */
		/* rfork(RFNAMEG); /1* keep mount of rio private *1/ */

		/* fd = open(srvwsys, ORDWR); */
		/* if(fd < 0) */
		/* 	sysfatal("can't open $wsys: %r"); */

		/* /1* mount creates window; see rio(4) *1/ */
		/* if(mount(fd, -1, "/tmp", MREPL, "new -dx 300-dy 200") < 0) */
		/* 	sysfatal("can't mount new window: %r"); */
		/* if(gengetwindow(display, "/tmp/winname", */
		/* 			&av_screen, &_av_screen, Refnone) < 0) */
		/* 	sysfatal("resize failed: %r"); */

		/* now open /tmp/cons, /tmp/mouse */

		av.amode = amode;
		poperror();
		aqunlock(&av);
		break;
	}
	c = devopen(c, omode, avdir, nelem(avdir), devgen);
	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;

	return c;
}

static void
avclose(Chan *c)
{
	switch((ulong)c->qid.path) {
	default:
		error(Eperm);
		break;

	case Qdir:
		break;

	case Qav:
		if(c->flag & COPEN) {
			aqlock(&av);

			/* avdevclose(); */

			av.amode = Vclosed;
			aqunlock(&av);
		}
		break;
	}
}

static long
avread(Chan *c, void *v, long n, vlong off)
{
	char *a;

	a = v;
	switch((ulong)c->qid.path) {
	default:
		error(Eperm);
		break;

	case Qdir:
		return devdirread(c, a, n, avdir, nelem(avdir), devgen);

	case Qav:
		if(av.amode != Vread)
			error(Emode);
		aqlock(&av);
		if(waserror()){
			aqunlock(&av);
			nexterror();
		}
		memset(a, 'A', n);
		poperror();
		aqunlock(&av);
		break;
	}
	return n;
}

static long
avwrite(Chan *c, void *vp, long n, vlong off)
{
	USED(off);
	switch((ulong)c->qid.path) {
	default:
		error(Eperm);
		break;

	case Qav:
		if(av.amode != Vwrite)
			error(Emode);
		aqlock(&av);
		if(waserror()){
			aqunlock(&av);
			nexterror();
		}

		/// This writes to the Screen (Display Screen?!)
		/* print("Hello from AV driver!\n"); */

		/// Can we replicate: "echo hello > /dev/cons" by opening /dev/cons in
		/// the context of the attached process group to the window (see
		/// rio(1)), section window control

		/// Connect to the created window in avopen() (see also graphics(2): gengetwindow() and getwindow())
		/// This results in a linker error: undefined reference to 'display'
		Display *disp = display;
		/* print("display pointer: %p", disp); */

		/// Then do the same with some simple graphics ...
		/// Need to devopen(c, ...) the draw device?
		/* static Rectangle r; */
		/* r = display->screenimage->r; */
		/* print("%R\n", r); */
		/* draw(display->screenimage, r, back, nil, ZP); */
		/* print("%R\n", r); */
		/* line(display->screenimage, r.min, r.max, 0, 0, 5, dline, ZP); */
		/* flushimage(display, 1); */

		poperror();
		aqunlock(&av);
		break;
	}
	return n;
}

Dev avdevtab = {
	'V',
	"av",

	devreset,
	avinit,
	devshutdown,
	avattach,
	avwalk,
	avstat,
	avopen,
	devcreate,
	avclose,
	avread,
	devbread,
	avwrite,
	devbwrite,
	devremove,
	devwstat,
};
