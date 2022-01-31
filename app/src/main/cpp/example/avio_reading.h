//
// Created by BZF on 2022/1/9.
//

#ifndef FFMPEGDEMO_AVIO_READING_H
#define FFMPEGDEMO_AVIO_READING_H

#include <stdint.h>
#include "../logutils.h"
#include <string>

extern "C"{
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavutil/file.h>
#include <libavcodec/avcodec.h>
};

struct BufferData{
    uint8_t *ptr; //数据
    size_t size; //数据大小
};

int init(char *inputFileName);

static int readPacket(void *opaque, uint8_t *buf, int bufSize);
#endif //FFMPEGDEMO_AVIO_READING_H
