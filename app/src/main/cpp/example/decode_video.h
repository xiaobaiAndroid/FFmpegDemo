//
// Created by BZF on 2022/1/10.
//

#ifndef FFMPEGDEMO_DECODE_VIDEO_H
#define FFMPEGDEMO_DECODE_VIDEO_H

#include "../logutils.h"

extern "C"{
#include <libavcodec/avcodec.h>
};

#define INBUFFER_SIZE 4096

/*
 * 把视频的一帧保存PGM(P5)格式的图片
 * */
static void savePGM(unsigned char *buffer, int wrap, int xsize, int ysize, char *filename);

static void decodeVideo(AVCodecContext *pCodecContext, AVFrame *pFrame, AVPacket *pPacket, const char *filename);

void startDecodeVideo(char *inputPath, char *outputDir);

#endif //FFMPEGDEMO_DECODE_VIDEO_H
