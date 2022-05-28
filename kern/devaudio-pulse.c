/* #include	<sndio.h> */
#include	<pulse/simple.h>
#include	<pulse/error.h>

#include	"u.h"
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"devaudio.h"

const pa_sample_spec ss = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};
pa_simple *s = NULL;

void
audiodevopen(void)
{
    int errno;
    if (!(s = pa_simple_new(NULL, "drawterm", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &errno))) {
        // TODO use pa_strerror(errno) for a more informative error message
        error("pa_simple_new() failed\n");
    }
}

void
audiodevclose(void)
{
    int errno;
    if (pa_simple_drain(s, &errno) < 0) {
        error("pa_simple_drain() failed");
    }
    if (s)
        pa_simple_free(s);
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
    if (pa_simple_write(s, v, (size_t) n, &errno) < 0 )
        error("pa_simple_write() failed");
    if (pa_simple_drain(s, &errno) < 0)
        error("pa_simple_drain() failed");
    return 0;
}

int
audiodevread(void *v, int n)
{
	error("no reading");
	return -1;
}
