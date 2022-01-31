//
// Created by BZF on 2022/1/18.
//

#include "encode_video.h"

YUVEncodeH264::YUVEncodeH264(char *outputPath) {
    mOutputPath = outputPath;
}



int YUVEncodeH264::initEncoder() {

    //查找H.264编码器
    mCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!mCodec){
        LOGI("未找到H264编码器");
        return -1;
    }
    mCodecContext = avcodec_alloc_context3(mCodec);
    if(!mCodecContext){
        LOGI("无法为编码器上下文分配空间");
        return -1;
    }

    mPacket = av_packet_alloc();
    if(!mPacket){
        LOGI("无法创建编码Packet");
        return -1;
    }
    mCodecContext->bit_rate = 400000;
    mCodecContext->width = 352;
    mCodecContext->height = 288;
    //25帧每秒
    mCodecContext->time_base = (AVRational){1, 25};
    mCodecContext->framerate = (AVRational){25, 1};

    mCodecContext->gop_size = 10;
    mCodecContext->max_b_frames = 1;
    mCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(mCodecContext->priv_data, "preset", "slow", 0);


    int result = avcodec_open2(mCodecContext, mCodec, nullptr);
    if(result < 0){
        LOGI("不能打开编码器");
        return -1;
    }


    mFrame = av_frame_alloc();
    if(!mFrame){
        LOGI("不能创建Frame");
        return -1;
    }
    mFrame->format = mCodecContext->pix_fmt;
    mFrame->width = mCodecContext->width;
    mFrame->height = mCodecContext->height;

    result  = av_frame_get_buffer(mFrame, 0);
    if(result < 0){
        LOGI("不能为视频数据分配新的缓冲区");
        return -1;
    }
    return 0;
}

int YUVEncodeH264::encode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *packet) {
    int result = avcodec_send_frame(codecContext, frame);
    if(result < 0){
        LOGI("发生帧数据给编码器时出错");
        return result;
    }
    while (result >= 0){
        result = avcodec_receive_packet(codecContext, packet);
        if(result == AVERROR(EAGAIN) || result == AVERROR_EOF){
            return result;
        }else if(result < 0){
            LOGI("编码时发生错误");
            return result;
        }
        fwrite(mPacket->data, 1, mPacket->size, mOutputFile);
        av_packet_unref(mPacket);
    }
    return 0;
}

void YUVEncodeH264::start() {

    mOutputFile = fopen(mOutputPath, "wb");
    if(!mOutputPath){
        LOGI("不能打开：%s", mOutputPath);
        return;
    }
    if(initEncoder() < 0){
        LOGI("初始化失败");
        return;
    }

    int result = 0;
    int y=0;
    int x=0;
    for (int i=0; i<25; i++){
        fflush(stdout); //刷新输出流

        //确保帧数据可写
        result = av_frame_make_writable(mFrame);
        if(result < 0){
            break;
        }

        //创建YUV图像
        //Y
        for (y = 0; y < mCodecContext->height; y++) {
            for (x = 0; x < mCodecContext->width; x++) {
                mFrame->data[0][y*mFrame->linesize[0] + x] = x + y + i *3;
            }
        }
        //Cb和Cr
        for (y = 0; y < mCodecContext->height / 2; y++) {
            for (x = 0; x < mCodecContext->width/2; x++) {
                mFrame->data[1][y * mFrame->linesize[1] + x] = 128 + y + i * 2;
                mFrame->data[2][y * mFrame->linesize[1] + x] = 64 + x + i * 5;
            }
        }

        //当前第几帧
        mFrame->pts = i;
        encode(mCodecContext, mFrame, mPacket);
    }
    encode(mCodecContext, nullptr, mPacket);
}


YUVEncodeH264::~YUVEncodeH264() {
    fclose(mOutputFile);

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

