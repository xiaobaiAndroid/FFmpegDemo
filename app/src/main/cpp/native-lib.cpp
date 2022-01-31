#include <jni.h>
#include <string>
#include <unistd.h>
#include "logutils.h"

#include "example/avio_reading.h"
#include "example/decode_audio.h"
#include "example/decode_video.h"
#include "example/demuxing_decoding.h"

#define OUT_PUT_CHANNELS 2

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_ffmpegInfo(JNIEnv *env, jobject thiz) {


    char info[40000] = {0};
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }

    return env->NewStringUTF(info);
}

JNIEXPORT jboolean JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_mp4ToAVI(JNIEnv *env, jobject thiz, jstring input_path,
                                              jstring output_path) {
//    AVOutputFormat *ofmt = NULL;
//    AVBSFContext *vbsf = NULL;
//    //定义输入、输出ACFormatContext
//    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
//    AVPacket pkt;
//    const char *in_filename, *out_filename;
//    int ret, i;
//    int frame_index = 0;
//
//    //4.0被弃用，不需要调用此函数
////    av_register_all();
//
//
//    in_filename = reinterpret_cast<const char *>(input_path);
//    out_filename = reinterpret_cast<const char *>(output_path);
//    //打开文件
//    ret = avformat_open_input(&ifmt_ctx,in_filename,NULL,NULL);
//    if(ret < 0){
//        LOGI("打开文件失败");
//        return false;
//    }
//
//    ret = avformat_find_stream_info(ifmt_ctx, NULL);
//    if(ret<0){
//        LOGI("获取视频信息失败");
//        return false;
//    }
//
//    //使用annexb封装模式
//    const AVBitStreamFilter *pFilter = av_bsf_get_by_name("h264_mp4toannexb");
//    av_bsf_alloc(pFilter, &vbsf);
//    av_bsf_init(vbsf);
//
//    //打印输入格式的详细信息
//    av_dump_format(ifmt_ctx,0,in_filename,0);
//
//    //初始化输出视频码流的AVFormatContext
//    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
//    if(!ofmt_ctx){
//        LOGI("Could not create output context\n");
//        ret = AVERROR_UNKNOWN;
//        return false;
//    }
//    ofmt = ofmt_ctx->oformat;
//    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
//        AVStream *in_stream = ifmt_ctx->streams[i];
//        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
//        if(!out_stream){
//            LOGI("分配输出流失败\n");
//            ret = AVERROR_UNKNOWN;
//            return false;
//        }
//        if(avcodec_copy_context(out_stream->codec,in_stream->codec) < 0){
//            LOGI("从输入流复制上下文到输出流编码器上下文失败");
//            return false;
//        }
//        out_stream->codec->codec_tag = 0;
//        //打开输出文件
//        if(!(ofmt_ctx->flags & AVFMT_NOFILE)){
//            ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
//            if(ret<0){
//                LOGI("不能打开输出文件：%s",out_filename);
//                return -1;
//            }
//        }
//        //写入头文件
//        if(avformat_write_header(ofmt_ctx,NULL) < 0){
//            LOGI("打开输出文件时发生错误");
//            return false;
//        }
//    }
//
//    avformat_close_input(&ifmt_ctx);
//    if(ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)){
//        avio_close(ofmt_ctx->pb);
//    }
//    avformat_free_context(ofmt_ctx);
//    av_bsf_free(&vbsf);
    return true;

}

JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_videoDecodePCMAndYUV(JNIEnv *env, jobject thiz,
                                                          jstring video_path, jstring pcm_url,
                                                          jstring yuv_url) {
//    const char *videoPath = env->GetStringUTFChars(video_path,NULL);
//    //1.注册协议、格式与编码器
//
//    //2.打开媒体文件源，并设置超时回调
//    AVFormatContext *pFormatContext = avformat_alloc_context();
//    avformat_open_input(&pFormatContext,videoPath,NULL,NULL);
//    int result = avformat_find_stream_info(pFormatContext, NULL);
//    if(result<0){
//        LOGI("获取视频信息失败");
//        return;
//    }
//
//    int videoStreamIndex=0, audioStreamIndex=0;
//    //3.寻找各个流，并且打开对应的解码器
//    for (int i = 0; i < pFormatContext->nb_streams; i++) {
//        AVStream *stream = pFormatContext->streams[i];
//        if(AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type){ //视频流
//            videoStreamIndex = i;
//        }else if(AVMEDIA_TYPE_AUDIO == stream->codecpar->codec_type){ //音频流
//            audioStreamIndex = i;
//        }
//    }
//    //打开音频解码器
//    AVStream *audioStream = pFormatContext->streams[audioStreamIndex];
//    AVCodecParameters *audioParameters = audioStream->codecpar;
//    AVCodecID audioCodecId = audioParameters->codec_id;
//    AVCodec *audioCodec = avcodec_find_decoder(audioCodecId);
//    if(!audioCodec){
//        LOGI("找不到对应的解码器");
//        return;
//    }
//
//    //打开视频解码器
//    AVStream *videoStream = pFormatContext->streams[videoStreamIndex];
//    AVCodecParameters *videoParamaters = videoStream->codecpar;
//    AVCodecID videoCodecId = videoParamaters->codec_id;
//    AVCodec *videoCodec = avcodec_find_decoder(videoCodecId);
//    if(!videoCodec){
//        LOGI("找不到对应的解码器");
//        return;
//    }
//
//    //4.初始化解码后数据的结构体
//    SwrContext *swrContext = NULL;
//    if(audioParameters->format != AV_SAMPLE_FMT_S16){
//        //如果不是我们需要的数据格式,转换成我们需要的格式
//
//        swrContext = swr_alloc_set_opts(NULL
//                           ,av_get_default_channel_layout(2)
//                           ,AV_SAMPLE_FMT_S16
//                           ,audioParameters->sample_rate
//                           ,av_get_default_channel_layout(audioParameters->channels)
//                           ,audioStream->codec->sample_fmt
//                           ,audioParameters->sample_rate
//                           ,0,NULL);
//        //创建swrContext结构体失败或者初始化swrContext失败
//        if(!swrContext || swr_init(swrContext)){
//            if(swrContext){
//                swr_free(&swrContext);
//            }
//        }
//    }
//    AVFrame *audioFrame = av_frame_alloc();
//
//    //构建视频的格式转换对象以及视频解码后数据存放的对象
//    AVPicture picture;
//    bool pictureValid = avpicture_alloc(&picture, AV_PIX_FMT_YUV420P, videoParamaters->width,
//                                        videoParamaters->height) == 0;
//    if(!pictureValid){
//        LOGI("分配失败");
//        return;
//    }
//    SwsContext *swsContext = NULL;
//    swsContext = sws_getCachedContext(swsContext //swsContext为NULL时，会开辟一块新的控件
//                         , videoParamaters->width //原始数据的宽
//                         ,videoParamaters->height
//                         ,videoStream->codec->pix_fmt //原始像素格式
//                         ,videoParamaters->width //目标数据的宽
//                         ,videoParamaters->height
//                         ,AV_PIX_FMT_YUV420P //目标像素格式
//                         ,SWS_FAST_BILINEAR //使用快速线性算法
//                         ,NULL,NULL,NULL);
//    AVFrame *videoFrame = av_frame_alloc();
//
//    //5.读取流并解码
//    AVPacket packet;
//    int gotFrame = 0;
//    while (true){
//        if(av_read_frame(pFormatContext,&packet)){ //读取帧数据
//            break;
//        }
//        int packetStreamIndex = packet.stream_index;
//        if(packetStreamIndex == videoStreamIndex){
//            int len = avcodec_decode_video2(videoStream->codec, videoFrame, &gotFrame, &packet);
//            if(len <0){
//                break;
//            }
//            if(gotFrame){
//
//            }
//
//        }else if(packetStreamIndex == audioStreamIndex){
//            int len = avcodec_decode_audio4(audioStream->codec, audioFrame, &gotFrame, &packet);
//            if(len <0){
//                break;
//            }
//            if(gotFrame){
//
//            }
//        }
//
//    }
//    //6.处理解码后的裸数据
//    void *audioData;
//    int numFrames;
//    void *swrBuffer;
//    if(swrContext){
//        int buffSize = av_samples_get_buffer_size(NULL, OUT_PUT_CHANNELS,
//                                                  (int) (audioFrame->nb_samples * OUT_PUT_CHANNELS),
//                                                  AV_SAMPLE_FMT_S16, 1);
//        if(!swrBuffer || )
//    }else{
//        audioData = audioFrame->data[0];
//        numFrames = audioFrame->nb_samples;
//    }
//    av_register_all();
//    //关闭所以资源
}


JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_readingAV(JNIEnv *env, jobject thiz, jstring av_url) {
    const char *url = env->GetStringUTFChars(av_url, nullptr);
    init(const_cast<char *>(url));
}

JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_decodeAudio(JNIEnv *env, jobject thiz, jstring input_path,
                                                 jstring output_path) {

    const char *inputPath = env->GetStringUTFChars(input_path, nullptr);
    const char *outputPath = env->GetStringUTFChars(output_path, nullptr);
    startDecodingMP2(inputPath, outputPath);
}

JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_decodeVideo(JNIEnv *env, jobject thiz, jstring input_path,
                                                 jstring output_dir) {
    char *inputPath = const_cast<char *>(env->GetStringUTFChars(input_path, nullptr));
    char *outputDir = const_cast<char *>(env->GetStringUTFChars(output_dir, nullptr));
    LOGI("inputPath=%s",inputPath);
    LOGI("outputDir=%s",outputDir);
    startDecodeVideo(inputPath,outputDir);

}
}


extern "C"
JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_demuxingDecoding(JNIEnv *env, jobject thiz, jstring input_path,
                                                      jstring output_dir) {
    char *inputPath = const_cast<char *>(env->GetStringUTFChars(input_path, nullptr));
    char *outputDir = const_cast<char *>(env->GetStringUTFChars(output_dir, nullptr));
    VideoDemuxingDecoding decoding(inputPath,outputDir);
    decoding.start();
}

#include "example/encode_audio.h"
extern "C"
JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_encodeAudio(JNIEnv *env, jobject thiz, jstring input_path,
                                                 jstring output_path) {
    char *inputPath = const_cast<char *>(env->GetStringUTFChars(input_path, nullptr));
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(output_path, nullptr));
    PCMEncodeMP2 pcmEncodeMp3(inputPath, outputPath);
    pcmEncodeMp3.start();
}

#include "example/encode_video.h"
extern "C"
JNIEXPORT void JNICALL
Java_com_bzf_ffmpegdemo_MainActivity_yuvEncodeH264(JNIEnv *env, jobject thiz,
                                                   jstring output_path) {
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(output_path, nullptr));

    YUVEncodeH264 encodeVideo(outputPath);
    encodeVideo.start();
}