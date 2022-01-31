//
// Created by BZF on 2022/1/11.
//

#ifndef FFMPEGDEMO_DEMUXING_DECODING_H
#define FFMPEGDEMO_DEMUXING_DECODING_H

#include "../logutils.h"

extern "C"{
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
};

/*
 * 视频解封装和解码
 *
 * */
class VideoDemuxingDecoding{
private:
    AVFormatContext  *pFormatContext = nullptr;
    AVCodecContext *pVideoCodecContext = nullptr;
    AVCodecContext *pAudioCodecContext = nullptr;
    int width;
    int height;
    enum AVPixelFormat pixelFormat;
    AVStream  *pVideoStream = nullptr;
    AVStream *pAudioStream = nullptr;
    char *pScrFileName = nullptr;
    char *pOutputDir = nullptr;
    char *pVideoDstFileName = nullptr;
    char *pAudioDstFileName = nullptr;
    FILE *pVideoDstFile = nullptr;
    FILE *pAudioDstFile = nullptr;
    uint8_t *pVideoDstData[4] = {nullptr};
    int videoDstLinesize[4];
    int videoDstBufferSize = 0;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    AVPacket *pPacket = nullptr;
    AVFrame *pFrame = nullptr;
    int videoFrameCount = 0;
    int audioFrameCount = 0;


    int outputVideoFrame(AVFrame *pFrame);
    int outputAudioFrame(AVFrame *pFrame);

    /*解码压缩数据包*/
    int decodePacket(AVCodecContext *pCodecContext, const AVPacket *pPacket);
    int openCodecContext(int *pStreamIndex, AVCodecContext **pCodecContext, AVFormatContext *pFormatContext, enum  AVMediaType type);
public:
    VideoDemuxingDecoding(char *inputFile,  char *outputDir);
    void start();
    char * joinStr(char *s1, char *s2);
    ~VideoDemuxingDecoding();
};

#endif //FFMPEGDEMO_DEMUXING_DECODING_H
