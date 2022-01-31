//
// Created by BZF on 2022/1/11.
//

#include "demuxing_decoding.h"

void VideoDemuxingDecoding::start() {
    //打开输入流并读取文件头
    int result = avformat_open_input(&pFormatContext, pScrFileName, nullptr, nullptr);
    if (result < 0) {
        LOGI("result=%d,不能打开：%s", result,pScrFileName);
        return;
    }

    //读取一个媒体文件的数据包以获得流信息
    result = avformat_find_stream_info(pFormatContext, nullptr);
    if (result < 0) {
        LOGI("无法获取流信息");
        return;
    }

    //获取视频解码器上下文
    result = openCodecContext(&videoStreamIndex, &pVideoCodecContext, pFormatContext,
                              AVMEDIA_TYPE_VIDEO);
    if (result >= 0) {
        pVideoStream = pFormatContext->streams[videoStreamIndex];
        pVideoDstFile = fopen(pVideoDstFileName, "wb");
        if (!pVideoDstFile) {
            LOGI("不能打开文件：", pVideoDstFileName);
            return;
        }
        //放置解码后的图像
        width = pVideoCodecContext->width;
        height = pVideoCodecContext->height;
        pixelFormat = pVideoCodecContext->pix_fmt;

        //为图片申请内存
        int size = av_image_alloc(pVideoDstData, videoDstLinesize, width, height, pixelFormat, 1);
        if (size < 0) {
            LOGI("无法分配原始视频缓冲区");
            return;
        }
        videoDstBufferSize = size;
    }

    //获取音频解码器上下文
    result = openCodecContext(&audioStreamIndex, &pAudioCodecContext, pFormatContext,
                              AVMEDIA_TYPE_AUDIO);
    if (result >= 0) {
        pAudioStream = pFormatContext->streams[audioStreamIndex];
        pAudioDstFile = fopen(pAudioDstFileName, "wb");
        if (!pAudioDstFile) {
            LOGI("无法打开目标文件：", pAudioDstFileName);
            return;
        }
    }

    //打印关于输入或输出格式的详细信息
    av_dump_format(pFormatContext, 0, pScrFileName, 0);

    if (!pAudioStream && !pVideoStream) {
        LOGI("在输入流中找不到音频流和视频流");
        return;
    }

    pFrame = av_frame_alloc();
    if (!pFrame) {
        LOGI("不能为帧分配内存");
        return;
    }
    pPacket = av_packet_alloc();
    if (!pPacket) {
        LOGI("不能为数据压缩包分配内存");
        return;
    }

    while ((av_read_frame(pFormatContext, pPacket)) >= 0) {
        if (pPacket->stream_index == videoStreamIndex) {
            //解码视频数据压缩包
            result = decodePacket(pVideoCodecContext, pPacket);
        } else if (pPacket->stream_index == audioStreamIndex) {
            result = decodePacket(pAudioCodecContext, pPacket);
        }
        //重置pPacket
        av_packet_unref(pPacket);
        if (result < 0) {
            break;
        }
    }

    //刷新解码器
    if (pVideoCodecContext) {
        decodePacket(pVideoCodecContext, nullptr);
    }
    if (pAudioCodecContext) {
        decodePacket(pAudioCodecContext, nullptr);
    }

    LOGI("解封装成功");

}


VideoDemuxingDecoding::VideoDemuxingDecoding(char *inputFile, char *outputDir) {
    this->pScrFileName = inputFile;
    this->pOutputDir = outputDir;

    this->pVideoDstFileName = joinStr(outputDir, "/output_raw.yuv");
    this->pAudioDstFileName = joinStr(outputDir, "/output_raw.pcm");
    LOGI("pVideoDstFileName=%s",pVideoDstFileName)
    LOGI("pAudioDstFileName=%s",pAudioDstFileName)
}


char* VideoDemuxingDecoding::joinStr(char *s1, char *s2)
{
    char *result = static_cast<char *>(malloc(strlen(s1) + strlen(s2) + 1));//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == nullptr) exit (1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

int VideoDemuxingDecoding::openCodecContext(int *pStreamIndex, AVCodecContext **pCodecContext,
                                            AVFormatContext *pFormatContext,
                                            enum AVMediaType type) {
    //在文件中找到指定类型的流的位置
    int result = av_find_best_stream(pFormatContext, type, -1, -1, nullptr, 0);
    const char *pMediaType = av_get_media_type_string(type);
    if (result < 0) {
        LOGI("输入文件中未找到 %s流", pMediaType);
        return result;
    }
    int streamIndex = result;

    AVStream *pStream = pFormatContext->streams[streamIndex];
    AVCodec *pCodec = avcodec_find_decoder(pStream->codecpar->codec_id);
    if (!pCodec) {
        LOGI("未找到 %s 的解码器", pMediaType);
        result = AVERROR(EINVAL);
        goto end;
    }
    *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!*pCodecContext) {
        LOGI("分配%s解码器上下文失败");
        //ENOMEM：内存不足
        result = AVERROR(ENOMEM);
        goto end;
    }
    //把流中的解码器中的参数值填充到解码器上下文
    result = avcodec_parameters_to_context(*pCodecContext, pStream->codecpar);
    if (result < 0) {
        LOGI("未能将%s编解码器参数复制到解码器上下文", pMediaType);
        goto end;
    }
    //打开解码器
    result = avcodec_open2(*pCodecContext, pCodec, nullptr);
    if (result < 0) {
        LOGI("打开%s解码器失败", pMediaType);
        goto end;
    }
    *pStreamIndex = streamIndex;
end:
    return result;
}

int VideoDemuxingDecoding::decodePacket(AVCodecContext *pCodecContext, const AVPacket *pPacket) {

    int result = avcodec_send_packet(pCodecContext, pPacket);
    if (result < 0) {
        LOGI("发送压缩数据包时出错：%s", av_err2str(result))
        return result;
    }
    while (result >= 0) {
        result = avcodec_receive_frame(pCodecContext, pFrame);
        if (result < 0) {
            //没有输出帧可用
            if (result == AVERROR_EOF || result == AVERROR(EAGAIN)) {
                return 0;
            }
            LOGI("解码时发生错误：%s", av_err2str(result));
            return result;
        }
        if (pCodecContext->codec->type == AVMEDIA_TYPE_VIDEO) {
            result = outputVideoFrame(pFrame);
        } else {
            result = outputAudioFrame(pFrame);
        }
        //取消引用所以帧引用的缓冲区， 并重置帧对象
        av_frame_unref(pFrame);
        if (result < 0) {
            return result;
        }
    }
    return 0;
}

int VideoDemuxingDecoding::outputAudioFrame(AVFrame *pFrame) {
    size_t unpadded_linesize = pFrame->nb_samples *
                               av_get_bytes_per_sample(static_cast<AVSampleFormat>(pFrame->format));
    LOGI("audio_frame n:%d nb_samples:%d pts:%s\n",
         audioFrameCount, pFrame->nb_samples,
         av_ts2timestr(pFrame->pts, &pAudioCodecContext->time_base))
    audioFrameCount++;
    /*
     *  写第一平面的原始音频数据样本。这是
    适用于打包格式(如AV_SAMPLE_FMT_S16)。然而,大多数音频解码器输出平面音频，其中使用单独的每个通道的音频样本平面(如AV_SAMPLE_FMT_S16P)。换句话说，这个代码将只写第一个音频通道
    在这些情况下。你应该使用libswresample或libavfilter来转换帧压缩数据。
     * */
    fwrite(pFrame->extended_data[0], 1, unpadded_linesize, pAudioDstFile);
    return 0;
}

int VideoDemuxingDecoding::outputVideoFrame(AVFrame *pFrame) {
    if(pFrame->width != width || pFrame->height != height || pFrame->format != pixelFormat){
        LOGI("raw(原始未编码的)视频文件中的宽度、高度、像素格式和输入视频的视频的不一样:\n"
             "raw: width=%d, height=%d, format=%s\n"
             "输入: width=%d, height=%d", width, height, av_get_pix_fmt_name(pixelFormat), pFrame->width
             ,pFrame->height, av_get_pix_fmt_name(pVideoCodecContext->pix_fmt))
        return -1;
    }
    LOGI("视频帧：%d, 编码图片数：%d", videoFrameCount,pFrame->coded_picture_number);
    videoFrameCount++;

    //将pFrame->data找那个的图像复制到pVideoDstData
    av_image_copy(pVideoDstData, videoDstLinesize, (const uint8_t **)(pFrame->data), pFrame->linesize, pixelFormat, width, height);

    //写入原始视频文件
    fwrite(pVideoDstData[0], 1, videoDstBufferSize,pVideoDstFile);
    return 0;
}


VideoDemuxingDecoding::~VideoDemuxingDecoding() {
    avcodec_free_context(&pVideoCodecContext);
    avcodec_free_context(&pAudioCodecContext);
    avformat_close_input(&pFormatContext);
    if(pVideoDstFile){
        fclose(pVideoDstFile);
    }
    if(pAudioDstFile){
        fclose(pAudioDstFile);
    }
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    av_free(pVideoDstData[0]);
    LOGI("资源已释放");

}