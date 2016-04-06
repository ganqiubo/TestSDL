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
int dex=1;


static void av_log_callback(void* ptr, int level, const char* fmt,va_list vl)
{
	 FILE *fp = fopen(out_file_log,"a+");
	    if(fp){
	        vfprintf(fp,fmt,vl);
	        fflush(fp);
	        fclose(fp);
	  }
	  LOGE("av_log_callback----->%s",fmt);
}


jstring Java_com_cc_encoding_WriteFile_encod(JNIEnv* env,jobject thiz){

	LOGE( "SDL_CreateWindow-----> %s", "kkkk");

	return (*env)->NewStringUTF(env, "oh");
}

void Java_com_cc_encoding_WriteFile_frameEncodeInit(JNIEnv* env,jobject thiz,jint w,jint h,jstring file,jstring fileLog){

	out_file=(*env)->GetStringUTFChars(env,file, 0);
	out_file_log=(*env)->GetStringUTFChars(env,fileLog, 0);

	av_log_set_callback(av_log_callback);

	av_register_all();

	avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	LOGE("filename----->%s", pFormatCtx->filename);

	fmt = av_guess_format(NULL, out_file, NULL);
	LOGE("--->%i",avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE));

	video_st= avformat_new_stream(pFormatCtx, 0);

	video_st->time_base.num = 1;
	video_st->time_base.den = 25;
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	LOGE("%i--->%i===>%i",AV_CODEC_ID_H264,AV_CODEC_ID_H265,fmt->video_codec);
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = PIX_FMT_YUV420P;//AV_PIX_FMT_NV21;
	pCodecCtx->width = w;
	pCodecCtx->height = h;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	//pCodecCtx->compression_level=1;
	pCodecCtx->bit_rate = 6*pCodecCtx->width*pCodecCtx->height;
	/*pCodecCtx->gop_size = 12;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;*/
	pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
	//AVDictionary *param = 0;
	//Show some Information
	LOGE("qmin：%s","");
	av_dump_format(pFormatCtx, 0, out_file, 1);
	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	LOGE("avcodec_open2---->%i",avcodec_open2(pCodecCtx, pCodec, NULL));
	avformat_write_header(pFormatCtx,NULL);
}

jbyteArray Java_com_cc_encoding_WriteFile_Writeframe(JNIEnv* env,jobject thiz,jbyteArray framedata){
	AVFrame *pFrame, *pFrameYUV;
	int len = (*env)->GetArrayLength(env, framedata);
	uint8_t *data=(*env)->GetByteArrayElements(env, framedata, 0);
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();


	avpicture_fill((AVPicture *)pFrame, data, AV_PIX_FMT_NV21,pCodecCtx->width, pCodecCtx->height);

	uint8_t *pBuffer;
	int numBytes;
	numBytes = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	pBuffer=(uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameYUV, pBuffer, pCodecCtx->pix_fmt,pCodecCtx->width, pCodecCtx->height);

	struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getContext(
			pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_NV21,
			pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0,
			pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

	int picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	//av_new_packet(&pkt,picture_size);
	av_init_packet(&pkt);
	pFrameYUV->pts=dex;
	int got_picture=-1;
	/*LOGE( "avcodec_encode_video2：%i--->got_picture：%i--->pkt.size--->%i",
			avcodec_encode_video2(pCodecCtx, &pkt,pFrame, &got_picture),got_picture,pkt.size);*/
	avcodec_encode_video2(pCodecCtx, &pkt,pFrameYUV, &got_picture);
	if (got_picture==1){
		pkt.stream_index = video_st->index;
		LOGE("pkt.size====>%i",pkt.size);
		av_write_frame(pFormatCtx, &pkt);
		dex++;
	}

	//uint8_t *------->jbyteArray
	jbyteArray byteArray=(*env)->NewByteArray(env,pkt.size);
	(*env)->SetByteArrayRegion(env,byteArray, 0, pkt.size, (jbyte *)pkt.data);

	av_free_packet(&pkt);
	av_free(pFrame);
	av_free(pFrameYUV);
	(*env)->ReleaseByteArrayElements(env, framedata, data, 0);

	return byteArray;
}

void Java_com_cc_encoding_WriteFile_WriteFinish(){
	LOGE("flush_encoder---->%i",flush_encoder(pFormatCtx,0));
	LOGE("av_write_trailer====>%i",av_write_trailer(pFormatCtx));
	if (video_st){
		avcodec_close(video_st->codec);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);
}

int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
        CODEC_CAP_DELAY))
        return 0;
    while (1) {
    	LOGE("Flushing stream #%u encoder\n", stream_index);
        //ret = encode_write_frame(NULL, stream_index, &got_frame);
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
            NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame)
        {ret=0;break;}
        LOGE("%s","编码成功1帧！\n");
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}
