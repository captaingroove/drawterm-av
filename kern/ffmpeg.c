#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/avstring.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include "drawterm.h"

/// TODO move all ffmpeg context to Client
static struct ffmpegctx
{
	AVFormatContext *pFormatCtx;
    AVIOContext     *pIOCtx;
    /* ByteQueue*      pRingBuffer; */
    unsigned char*  pInputBuffer;
    int inputBufferSize;
} ffmpegctx;

int
demuxerPacketRead(void* pRingBuffer, uint8_t* buf, int buf_size)
{
  /* LOG("reading %d bytes", buf_size); */
  /// TODO ...
  /* auto pFifo = reinterpret_cast<ByteQueue*>(pRingBuffer); */
  /* int bytesRead = pFifo->readSome((char*)buf, buf_size); */
  /* return bytesRead; */
  return 0;
}

int
initffmpeg()
{
	int ret = -1;
    ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    if (ret != 0)
    {
        LOG("Could not initialize SDL - %s", SDL_GetError());
        return -1;
    }
    ffmpegctx.pFormatCtx = NULL;
    ffmpegctx.inputBufferSize = 8192;
	ffmpegctx.pIOCtx = avio_alloc_context(
	    ffmpegctx.pInputBuffer,        // buffer
	    ffmpegctx.inputBufferSize,     // buffer size
	    0,                             // buffer is only readable - set to 1 for read/write
	    /* TODO pRingBuffer,           // user specified data */
	    0,                             // user specified data
	    demuxerPacketRead,             // function for reading packets
	    NULL,                          // function for writing packets
	    NULL                           // function for Seeking to position in stream
	    );
	if(!ffmpegctx.pIOCtx){
	  LOG("failed to allocate memory for ffmpeg av io context");
	  return 1;
	}
	return 0;
}

int
deinitffmpeg()
{
	return 0;
}