#include <u.h>
/* #include <libc.h> */
/* #include <fcall.h> */
#include <pthread.h>
/* #include <9p.h> */
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"

void
fsread(Req *r)
{
	readstr(r, "Hello from libav!\n");
	respond(r, nil);
}

Srv fs = {
	.read = fsread,
};

void
main(void)
{
	Tree *tree;

	tree = alloctree(nil, nil, DMDIR|0555, nil);
	fs.tree = tree;
	createfile(tree->root, "data", nil, 0555, nil);

	postmountsrv(&fs, nil, "/mnt/av", MREPL | MCREATE);
}
