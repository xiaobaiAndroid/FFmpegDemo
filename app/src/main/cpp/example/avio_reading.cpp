//
// Created by BZF on 2022/1/9.
//

#include "avio_reading.h"

static int readPacket(void *opaque, uint8_t *buf, int bufSize) {
    BufferData *buffData = (struct BufferData *) opaque;
    bufSize = FFMIN(bufSize, bufSize);
    if (!bufSize) {
        return AVERROR_EOF;
    }
    LOGI("ptr=%p size=%zu\n", buffData->ptr, buffData->size);
    memcpy(buf, buffData->ptr, bufSize);
    buffData->ptr += bufSize;
    buffData->size -= bufSize;
    return bufSize;
}

int init(char *inputFileName) {
    AVFormatContext *formatContext = nullptr;
    AVIOContext *avioContext = nullptr;
    uint8_t *buffer = nullptr, *avioContextBuffer = nullptr;
    size_t bufferSize, avioContextBufferSize = 4096;
    struct BufferData bufferData = {0};

    //读取文件，并将数据放到buffer缓冲区
    int ret = av_file_map(inputFileName, &buffer, &bufferSize, 0, nullptr);
    if (ret < 0) {
        goto end;
    }
    bufferData.ptr = buffer;
    bufferData.size = bufferSize;

    if (!(formatContext = avformat_alloc_context())) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    avioContextBuffer = static_cast<uint8_t *>(av_malloc(avioContextBufferSize));
    if (!avioContextBuffer) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    avioContext = avio_alloc_context(avioContextBuffer, avioContextBufferSize, 0,
                                     &bufferData,
                                     &readPacket, nullptr, nullptr);
    if (!avioContext) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    formatContext->pb = avioContext;
    ret = avformat_open_input(&formatContext, nullptr, nullptr, nullptr);
    if(ret < 0){
        LOGI("不能打开输入流\n");
        goto end;
    }

    ret = avformat_find_stream_info(formatContext, nullptr);
    if(ret < 0){
        LOGI("无法读取媒体信息");
        goto end;
    }
    av_dump_format(formatContext, 0, inputFileName, 0);

    end:
    avformat_close_input(&formatContext);
    if(avioContext){
        av_freep(&avioContext);
    }
    avio_context_free(&avioContext);
    av_file_unmap(buffer, bufferSize);

    return 0;
}