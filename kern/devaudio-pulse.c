/* #include	<sndio.h> */
#include	<pulse/simple.h>
#include	<pulse/error.h>

#include	"u.h"
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"devaudio.h"

/* enum */
/* { */
/* 	Channels = 2, */
/* 	Rate = 44100, */
/* 	Bits = 16, */
/* }; */
/* static struct sio_hdl *hdl; */
/* static struct sio_par par; */

const pa_sample_spec ss = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};
pa_simple *s = NULL;

void
audiodevopen(void)
{
    /* return; */
    int errno;
    if (!(s = pa_simple_new(NULL, "drawterm", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &errno))) {
        /* error("pa_simple_new() failed: %s\n", pa_strerror(errno)); */
        error("pa_simple_new() failed\n");
    }
	/* hdl = sio_open(SIO_DEVANY, SIO_PLAY, 0); */
	/* if(hdl == NULL){ */
	/* 	error("sio_open failed"); */
	/* 	return; */
	/* } */

	/* sio_initpar(&par); */

	/* par.bits = Bits; */
	/* par.pchan = Channels; */
	/* par.rate = Rate; */
	/* par.appbufsz = 288000; */

	/* if(!sio_setpar(hdl, &par) || !sio_start(hdl)){ */
	/* 	sio_close(hdl); */
	/* 	error("sio_setpar/sio_start failed"); */
	/* 	return; */
	/* } */
}

void
audiodevclose(void)
{
    int errno;
    if (pa_simple_drain(s, &errno) < 0) {
        error("pa_simple_drain() failed");
        /* fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(errno)); */
        /* goto finish; */
    }
    if (s)
        pa_simple_free(s);
	/* sio_close(hdl); */
}

void
audiodevsetvol(int what, int left, int right)
{
	USED(what);
	USED(left);
	USED(right);
	error("not supported");
}

void
audiodevgetvol(int what, int *left, int *right)
{
	USED(what);
	USED(left);
	USED(right);
	error("not supported");
}

int
audiodevwrite(void *v, int n)
{
    int errno;
    return pa_simple_write(s, v, (size_t) n, &errno);
    /* if (pa_simple_write(s, v, (size_t) n, &error) < 0) { */
    /*     error("pa_simple_write() failed\n"); */
    /*     /1* fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error)); *1/ */
    /* } */
	/* return sio_write(hdl, v, n); */
}

int
audiodevread(void *v, int n)
{
	error("no reading");
	return -1;
}
