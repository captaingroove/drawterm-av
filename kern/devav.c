#include	"u.h"
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"devav.h"

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
		/* n = avdevread(v, n); */
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
		/* n = avdevwrite(vp, n); */
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
