//
// Created by BZF on 2022/1/18.
//

#ifndef FFMPEGDEMO_ENCODE_VIDEO_H
#define FFMPEGDEMO_ENCODE_VIDEO_H

#include "../logutils.h"
extern "C"{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
};

#define INBUFFER_SIZE 4096

/*
 * YUV格式数据编码成视频
 * */
class YUVEncodeH264{
private:

    AVCodecContext *mCodecContext = nullptr;
    AVCodec *mCodec = nullptr;
    AVFrame *mFrame = nullptr;
    AVPacket *mPacket = nullptr;
    FILE *mOutputFile = nullptr;
    char *mOutputPath = nullptr;

    int initEncoder();
    int encode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *packet);
public:
    YUVEncodeH264(char *outputPath);
    void start();
    ~YUVEncodeH264();
};

#endif //FFMPEGDEMO_ENCODE_VIDEO_H
