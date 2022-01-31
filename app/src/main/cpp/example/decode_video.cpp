//
// Created by BZF on 2022/1/10.
//

#include "decode_video.h"


static void savePGM(unsigned char *buffer, int wrap, int xsize, int ysize, char *filename){
    FILE *f;
    int i;

    f = fopen(filename,"wb");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buffer + i * wrap, 1, xsize, f);
    fclose(f);
}

static void decodeVideo(AVCodecContext *pCodecContext, AVFrame *pFrame, AVPacket *pPacket, const char *filename){

    char buffer[1024];
    int result = avcodec_send_packet(pCodecContext, pPacket);
    if(result < 0){
        LOGI("发送解码数据包时出错: %d", result);
        throw "发送解码数据包时出错";
    }
    while (result >= 0){
        result = avcodec_receive_frame(pCodecContext, pFrame);
        if(result == AVERROR(EAGAIN) || result == AVERROR_EOF){
            return;
        }else if(result < 0){
            LOGI("解码时出错");
            return;
        }
        LOGI("保存帧：%3d", pCodecContext->frame_number)
        //刷新输出流， 把缓存区的数据写入到输出设备
        fflush(stdout);
        //设置帧的文件名并放到buffer字符数组中
        snprintf(buffer, sizeof(buffer), "%s-%d.PGM", filename,pCodecContext->frame_number);
        savePGM(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, buffer);
    }
}

void startDecodeVideo(char *inputPath, char *outputDir){
    uint8_t buffer[INBUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data = nullptr;
    size_t dataSize = 0;
    int result = 0;
    AVCodecParserContext *pParserContext = nullptr;
    AVCodecContext *pCodecContext = nullptr;
    FILE *pFile= nullptr;
    AVFrame *pFrame = nullptr;
    char *outputPath;


    AVPacket *pPacket = av_packet_alloc();
    if(!pPacket){
        return;
    }
    AVCodec *pCodec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if(!pCodec){
        goto end;
    }
    pParserContext = av_parser_init(pCodec->id);
    if(!pParserContext){
        LOGI("未找到解析器");
        goto end;
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    if(!pCodecContext){
        LOGI("无法创建视频解码器上下文");
        goto end;
    }

    if(avcodec_open2(pCodecContext,pCodec, nullptr) < 0){
        LOGI("不能打开解码器");
        goto end;
    }

    pFile = fopen(inputPath, "rb");
    if(!pFile){
        LOGI("不能打开：%s",inputPath);
        goto end;
    }
    pFrame = av_frame_alloc();
    if(!pFrame){
        LOGI("无法分配视频帧");
        goto end;
    }
    outputPath = strcat(outputDir, "/frame");

    while (!feof(pFile)){ //是否到达文件末尾
        dataSize = fread(buffer, 1, INBUFFER_SIZE, pFile);

        if(!dataSize){//没有数据，跳出循环
            break;
        }
        data = buffer;
        while (dataSize > 0){
           result =  av_parser_parse2(pParserContext, pCodecContext, &pPacket->data, &pPacket->size, data, dataSize,AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
           if(result < 0){
               LOGI("解析时发生错误");
               goto end;
           }
           //移动指针
           data += result;
           dataSize -= result;

           if(pPacket->size){
               try {
                   decodeVideo(pCodecContext, pFrame, pPacket, outputPath);
               } catch (const char *msg) {
                   goto end;
               }

           }
        }
    }
    //刷新解码器
    decodeVideo(pCodecContext, pFrame, pPacket, outputDir);
end:
    LOGI("释放资源");
    fclose(pFile);
    av_parser_close(pParserContext);
    avcodec_free_context(&pCodecContext);
    av_frame_free(&pFrame);
    av_packet_free(&pPacket);

}