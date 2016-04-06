#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <string.h>
#include <jni.h>
#include <time.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil\log.h>
#define LOG_TAG "GQB_encoding"
#undef LOG
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
int hh=1;

AVFormatContext* pFormatCtx;
AVOutputFormat* fmt;
AVStream* video_st;
AVCodecContext* pCodecCtx;
AVCodec* pCodec;
AVPacket pkt;
const char* out_file;
const char* out_file_log;
//const char* out_file = "/storage/sdcard0/src01.mp4";














