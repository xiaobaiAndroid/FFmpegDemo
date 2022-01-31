//
// Created by BZF on 2022/1/10.
//

#include "decode_audio.h"

static void
decodeAudio(AVCodecContext *avCodecContext, AVPacket *packet, AVFrame *frame, FILE *outfile) {
    int result, dataSize;
    int i, channel;

    //将带有压缩数据的数据包发送给解码器
    result = avcodec_send_packet(avCodecContext, packet);
    if (result < 0) {
        LOGI("向解码器提交数据包时出错");
        return;
    }
    //读取所以输出帧
    while (result >= 0) {
        result = avcodec_receive_frame(avCodecContext, frame);

        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return;
        } else if (result < 0) {
            LOGI("解码时发生错误");
            return;
        }
        dataSize = av_get_bytes_per_sample(avCodecContext->sample_fmt);
        if (dataSize < 0) {
            LOGI("计算位深度大小失败");
            return;
        }
        for (i = 0; i < frame->nb_samples; i++) {
            for (channel = 0; channel < avCodecContext->channels; channel++) {
                fwrite(frame->data[channel] + dataSize * i, 1, dataSize, outfile);
            }
        }
    }
}

/*
 * 解码出来的PCM在Mac可以用Audition播放
 *
 * 1.声明AVPacket对象，并分配内存
 * 2.查找解码器
 * 3.获取解码器上下文
 * 4.声明媒体上下文，并分配内存
 * 5.打开解码器
 * 5.打开音频文件
 * 6.打开要保存解码数据的文件
 * 7.读取数据解码
 * 8.释放资源
 * */
void startDecodingMP2(const char *inputPath, const char *outputPath) {
    AVPacket *pAvPacket = av_packet_alloc();

    AVCodec *pAvCodec = avcodec_find_decoder(AV_CODEC_ID_MP2);
    if (!pAvCodec) {
        LOGI("解码器未找到");
        return;
    }
    AVCodecParserContext *pParserContext = av_parser_init(pAvCodec->id);
    if (!pParserContext) {
        LOGI("解码器上下文未找到");
        return;
    }

    AVCodecContext *pCodecContext = avcodec_alloc_context3(pAvCodec);
    if (!pCodecContext) {
        LOGI("不能分配音频解码器上下文");
        return;
    }
    if (avcodec_open2(pCodecContext, pAvCodec, nullptr) < 0) {
        LOGI("无法打开解码器")
        return;
    }
    FILE *pFile = fopen(inputPath, "rb");
    if (!pFile) {
        LOGI("不能打开：%s", inputPath);
        return;
    }
    FILE *pOutputFile = fopen(outputPath, "wb");
    if (!pOutputFile) {
        avcodec_free_context(&pCodecContext);
        return;
    }

    AVFrame *pDecodedFrame = av_frame_alloc();
    if (!pDecodedFrame) {
        LOGI("不能分配音频帧");
        return;
    }

    uint8_t buffer[AUDIO_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data = buffer;
    size_t dataSize = fread(data, 1, AUDIO_BUFFER_SIZE, pFile);

    int len = 0;

    while (dataSize > 0) {
        int bytesCount = av_parser_parse2(pParserContext, pCodecContext, &pAvPacket->data,
                                          &pAvPacket->size, data, dataSize, AV_NOPTS_VALUE,
                                          AV_NOPTS_VALUE, 0);

        if (bytesCount < 0) {
            LOGI("解析错误");
            break;
        }
        data += bytesCount;
        dataSize -= bytesCount;

        if(pAvPacket->size){//有数据
            LOGI("通道数：%d, 采样率：%d",pCodecContext->channels, pCodecContext->sample_rate, pCodecContext->sample_fmt)

            decodeAudio(pCodecContext, pAvPacket, pDecodedFrame,pOutputFile);
        }
        if(dataSize < AUDIO_REFILL_THRESH){
            //从data复制dataSize个字节到buffer
            memmove(buffer, data, dataSize);
            data = buffer;
            len = fread(data + dataSize, 1, AUDIO_BUFFER_SIZE - dataSize, pFile);
            if(len > 0){
                dataSize += len;
            }
        }
    }

    //刷新解码器
    pAvPacket->data = NULL;
    pAvPacket->size = 0;
    decodeAudio(pCodecContext, pAvPacket, pDecodedFrame, pOutputFile);


    fclose(pOutputFile);
    fclose(pFile);
    avcodec_free_context(&pCodecContext);
    av_parser_close(pParserContext);
    av_frame_free(&pDecodedFrame);
    av_packet_free(&pAvPacket);
}