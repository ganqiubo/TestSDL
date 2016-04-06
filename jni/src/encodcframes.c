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

AVCodecContext* pCodecCtx;
AVCodec* pCodec;
AVPacket pkt;
const char* out_file_log;
//const char* out_file = "/storage/sdcard0/src01.mp4";

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

void Java_com_cc_encoder_EncodingFrames_frameEncodeInit(JNIEnv* env,jobject thiz,jint w,jint h,jstring fileLog){

	out_file_log=(*env)->GetStringUTFChars(env,fileLog, 0);

	av_log_set_callback(av_log_callback);

	av_register_all();
	pCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
	if(pCodec==NULL){
		LOGE("frameEncodeInit----->%s","nullllllll");
	}
	pCodecCtx=avcodec_alloc_context3(pCodec);
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = PIX_FMT_YUV420P;//AV_PIX_FMT_NV21;
	pCodecCtx->width = w;
	pCodecCtx->height = h;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	pCodecCtx->bit_rate = 10*pCodecCtx->width*pCodecCtx->height;
	pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
	LOGE("frameEncodeInit----->%i",pCodecCtx->height);
	pCodecCtx->max_b_frames=0;
	/*pCodecCtx->p_count=0;*/
	LOGE("avcodec_open2---->%i",avcodec_open2(pCodecCtx, pCodec, NULL));

}

jbyteArray Java_com_cc_encoder_EncodingFrames_encodcframe(JNIEnv* env,jobject thiz,jbyteArray framedata){
	int64_t t1= av_gettime();
	AVFrame *pFrame, *pFrameYUV;
	int len = (*env)->GetArrayLength(env, framedata);
	uint8_t *data=(*env)->GetByteArrayElements(env, framedata, 0);

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	avpicture_fill((AVPicture *) pFrame, data, AV_PIX_FMT_NV21,
			pCodecCtx->width, pCodecCtx->height);

	uint8_t *pBuffer;
	int numBytes;
	numBytes = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width,
			pCodecCtx->height);
	pBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *) pFrameYUV, pBuffer, pCodecCtx->pix_fmt,
			pCodecCtx->width, pCodecCtx->height);

	struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			AV_PIX_FMT_NV21, pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	sws_scale(img_convert_ctx, (const uint8_t* const *) pFrame->data,
			pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
			pFrameYUV->linesize);

	av_init_packet(&pkt);
	int got_picture=-1;

	//编码为I帧
	pFrameYUV->pict_type=AV_PICTURE_TYPE_I;
	pFrameYUV->key_frame=1;

	//LOGE("pkt.size====>%i",111);
	avcodec_encode_video2(pCodecCtx, &pkt,pFrameYUV, &got_picture);
	//if (got_picture==1){
		//LOGE("pkt.size====>%i",pkt.size);
	//}
	//LOGE("pkt.size====>%i",pkt.size);

	//uint8_t *------->jbyteArray
	jbyteArray byteArray=(*env)->NewByteArray(env,pkt.size);
	(*env)->SetByteArrayRegion(env,byteArray, 0, pkt.size, (jbyte *)pkt.data);

	int64_t t2= av_gettime();
	LOGE("%i====>%lld",pkt.size,(t2-t1));

	av_free_packet(&pkt);
	av_free(pFrame);
	av_free(pFrameYUV);
	(*env)->ReleaseByteArrayElements(env, framedata, data, 0);

	return byteArray;
}










