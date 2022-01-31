//
// Created by BZF on 2022/1/17.
//

#include "encode_audio.h"

PCMEncodeMP2::PCMEncodeMP2(char *inputPath, char *outputPath) {
    mInputPath = inputPath;
    mOutputPath = outputPath;
}

int PCMEncodeMP2::start() {
    mCodec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if(!mCodec){
        LOGI("未找到MP3编码器");
        return -1;
    }
    mCodecContext = avcodec_alloc_context3(mCodec);
    if(!mCodecContext){
        LOGI("不能为编码器上下文开辟空间");
        return -1;
    }
    //设置MP3的比特率 采样精度*采样率*声道数=比特率(码率)
    mCodecContext->bit_rate = 64000;
    //设置采样精度
    mCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    if(!checkSampleFormat(mCodecContext->sample_fmt)){
        LOGI("编码器不支持%s采样精度", av_get_sample_fmt_name(mCodecContext->sample_fmt));
        return -1;
    }

    mCodecContext->sample_rate = selectSampleRate();
    mCodecContext->channel_layout = selectChannelLayout();
    mCodecContext->channels = av_get_channel_layout_nb_channels(mCodecContext->channel_layout);

    int result = avcodec_open2(mCodecContext, mCodec, nullptr);
    if(result< 0){
        LOGI("不能打开解码器");
        return -1;
    }

//    mInputFile = fopen(mInputPath, "rb");
//    if(!mInputFile){
//        LOGI("不能打开：%s",mInputPath);
//        return -1;
//    }

    mOutputFile = fopen(mOutputPath, "wb");
    LOGI("mOutputPath=%s",mOutputPath);
    if(!mOutputFile){
        LOGI("不能打开：%s", mOutputPath);
        return -1;
    }

    mPacket = av_packet_alloc();
    if(!mPacket){
        LOGI("无法给Packet分配内存");
        return -1;
    }

    mFrame = av_frame_alloc();
    if(!mFrame){
        LOGI("无法给Frame分配内存");
        return -1;
    }
    mFrame->nb_samples = mCodecContext->frame_size;
    mFrame->format = mCodecContext->sample_fmt;
    mFrame->channel_layout = mCodecContext->channel_layout;

    result = av_frame_get_buffer(mFrame, 0);
    if(result < 0){
        LOGI("无法为音频帧数据分配新的缓冲区");
        return -1;
    }

     //编码成单声道MP2
    float t = 0;
    float tincr = 2 * M_PI * 440.0 / mCodecContext->sample_rate;
    uint16_t *samples;
    for (int i = 0; i < 200; i++) {
        //确保帧数据可写
        result = av_frame_make_writable(mFrame);
        if (result < 0) {
            LOGI("帧数据不可写");
            return -1;
        }

        samples = (uint16_t*)mFrame->data[0];

        //生成PCM音频数据
        for (int j = 0; j < mCodecContext->frame_size; j++) {
            samples[2*j] = (int)(sin(t) * 10000);
            for (int k = 1; k < mCodecContext->channels; k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }
        encode(mCodecContext, mFrame, mPacket, mOutputFile);
    }

    encode(mCodecContext, nullptr,mPacket,mOutputFile);

    LOGI("编码完成");
    return 0;
}

int PCMEncodeMP2::checkSampleFormat(enum AVSampleFormat targetSF) {
    const enum AVSampleFormat *sampleFormat = mCodec->sample_fmts;
    while (*sampleFormat != AV_SAMPLE_FMT_NONE){
        LOGI("sampleFormat=%s", av_get_sample_fmt_name(*sampleFormat))
        if(*sampleFormat == targetSF){
            return 1;
        }
        sampleFormat++;
    }
    return 0;
}

uint64_t PCMEncodeMP2::selectChannelLayout() {
    if(!mCodec->channel_layouts){
        //立体声
        return AV_CH_LAYOUT_STEREO;
    }
    const uint64_t *channelLayouts = mCodec->channel_layouts;
    uint64_t bestChannelLayout = 0;
    int bestChannelNumber = 0;

    //取得最大的通道数
    while (*channelLayouts){
        int nbChannels = av_get_channel_layout_nb_channels(*channelLayouts);
        if(nbChannels > bestChannelNumber){
            bestChannelLayout = *channelLayouts;
            bestChannelNumber = nbChannels;
        }
        channelLayouts++;
    }
    return bestChannelLayout;

}

int PCMEncodeMP2::selectSampleRate() {
    //判断编码器是否支持采样
    if(!mCodec->supported_samplerates){
        return 44100;
    }
    const int *samplerates = mCodec->supported_samplerates;
    int bestSamplerate = 0;
    //循环数组，取得最佳采样值
    while (*samplerates){
        LOGI("编码器支持的采样: %d", *samplerates);
        if(!bestSamplerate || abs(44100 - *samplerates) < abs(44100 - bestSamplerate)){
            bestSamplerate = *samplerates;
        }
        samplerates++;
    }
    LOGI("最佳采样率: %d", bestSamplerate);
    return bestSamplerate;
}

void PCMEncodeMP2::encode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *packet,
                          FILE *output) {

    int result = avcodec_send_frame(codecContext, frame);
    if(result < 0){
        LOGI("将帧发送到编码器时出错");
        return;
    }
    while (result >= 0){
        result = avcodec_receive_packet(codecContext, packet);
        if(result == AVERROR(EAGAIN) || result == AVERROR_EOF){ //没有数据或者到达文件结束符
            return;
        }else if(result < 0){
            LOGI("编码音频帧发生错误");
            return;
        }
        fwrite(packet->data, 1, packet->size, output);

        //重置packet
        av_packet_unref(packet);
    }
}

PCMEncodeMP2::~PCMEncodeMP2() {
    if(mOutputFile){
        fclose(mOutputFile);
    }
    if(mInputFile){
        fclose(mInputFile);
    }
    if(mFrame){
        av_frame_free(&mFrame);
    }

    if(mPacket){
        av_packet_free(&mPacket);
    }

    if(mCodecContext){
        avcodec_free_context(&mCodecContext);
    }
}