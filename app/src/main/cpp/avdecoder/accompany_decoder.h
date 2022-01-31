//
// Created by BZF on 2022/1/9.
//

#ifndef FFMPEGDEMO_ACCOMPANY_DECODER_H
#define FFMPEGDEMO_ACCOMPANY_DECODER_H

typedef struct AudioPacket{

} AudioPacket;

extern "C" {
#include "../ffmpeg/include/libavcodec/avcodec.h"
#include "../ffmpeg/include//libavformat/avformat.h"
#include "../ffmpeg/include/libavutil/avutil.h"
#include "../ffmpeg/include/libavutil/common.h"
#include "../ffmpeg/include/libavutil/channel_layout.h"
#include "../ffmpeg/include/libavutil/opt.h"
#include "../ffmpeg/include/libavutil/imgutilsh"

}

#endif //FFMPEGDEMO_ACCOMPANY_DECODER_H
