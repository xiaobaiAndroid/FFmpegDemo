//
// Created by BZF on 2022/1/17.
//

#ifndef FFMPEGDEMO_ENCODE_AUDIO_H
#define FFMPEGDEMO_ENCODE_AUDIO_H

#include "../logutils.h"
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
};

/*
 * 输入PCM,输出MP2
 * */
class PCMEncodeMP2{
private:
    AVCodecContext *mCodecContext = nullptr;
    AVCodec *mCodec = nullptr;
    AVFrame *mFrame = nullptr;
    AVPacket *mPacket = nullptr;
    AVFormatContext *mFormatCtx = nullptr;

    char *mOutputPath = nullptr;
    char *mInputPath = nullptr;
    FILE *mInputFile = nullptr;
    FILE *mOutputFile = nullptr;


    //检查编码器是否支持给定的采样精度
    int checkSampleFormat(enum AVSampleFormat targetSF);
    //选择采样率
    int selectSampleRate();
    //选择通道数
    uint64_t selectChannelLayout();
    //开始编码
    void encode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *packet,
                FILE *output);

public:
    PCMEncodeMP2(char *inputPath, char *outputPath);
    int start();
    ~PCMEncodeMP2();
};

#endif //FFMPEGDEMO_ENCODE_AUDIO_H
