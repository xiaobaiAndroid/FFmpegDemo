//
// Created by BZF on 2022/1/10.
//

#ifndef FFMPEGDEMO_DECODE_AUDIO_H
#define FFMPEGDEMO_DECODE_AUDIO_H

#include "../logutils.h"

extern "C"{
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
};

//设置音频缓存的最大值20M
#define AUDIO_BUFFER_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

static int getFormatFromSampleFmt(const char **fmt, enum AVSampleFormat sampleFormat);

//解码并写入指定文件
static void decodeAudio(AVCodecContext *avCodecContext, AVPacket *packet, AVFrame *frame, FILE *outfile);

void startDecodingMP2(const char *inputPath, const char *outputPath);

#endif //FFMPEGDEMO_DECODE_AUDIO_H
