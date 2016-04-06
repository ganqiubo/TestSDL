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
int num=1;
AVPacket dts;
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

void Java_com_cc_decodec_DecodecFrames_decodecFrameInit(JNIEnv* env,jobject thiz,jint w,jint h,jstring fileLog){
	out_file_log=(*env)->GetStringUTFChars(env,fileLog, 0);

		av_log_set_callback(av_log_callback);

		av_register_all();
		pCodec=avcodec_find_decoder(AV_CODEC_ID_MPEG4);
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
		pCodecCtx->max_b_frames=0;
		pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
		LOGE("frameEncodeInit----->%i",pCodecCtx->height);

		LOGE("//avcodec_open2---->%i",avcodec_open2(pCodecCtx, pCodec, NULL));
}

void Java_com_cc_decodec_DecodecFrames_decodecFrame(JNIEnv* env,jobject thiz,jbyteArray framedata){
	AVPacket pkt;
	AVFrame *avframe=NULL;
	AVFrame *pFrameRGB = NULL;

	int len = (*env)->GetArrayLength(env, framedata);
	uint8_t *pktdata=(*env)->GetByteArrayElements(env, framedata, 0);

	//pkt = (AVPacket *)av_malloc(sizeof(AVPacket));//分配内存
	avframe=av_frame_alloc();//分配内存
	pFrameRGB = av_frame_alloc();//分配内存
	//got_picture_ptr=(int *)av_malloc(sizeof(int));
	uint8_t *pBuffer;
	int numBytes;
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	pBuffer=(uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameRGB, pBuffer, AV_PIX_FMT_RGB24,pCodecCtx->width, pCodecCtx->height);

	av_init_packet(&pkt);
	int got_picture=-1;
	pkt.data=pktdata;
	pkt.size=len;
	//if(avframe->key_frame==0){
	//LOGE("av_copy_packet_side_data---->%i",av_copy_packet_side_data(&dts,&pkt));
	//}
	//LOGE("len---->%i::::",11111);
	avcodec_decode_video2(pCodecCtx, avframe,&got_picture, &pkt);
	//LOGE("len---->%i::::",22222);
	struct SwsContext *img_convert_ctx;
	if(got_picture){
		img_convert_ctx = sws_getContext(
				pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
				pCodecCtx->width, pCodecCtx->height,
				AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);

		sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data, avframe->linesize, 0,
				pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

		/*if(avframe->key_frame==1){
			dts=pkt;
			LOGE("len---->%s","dts<<<<pkt");
		}*/
		char* s1="/storage/sdcard1/pic";
		//char* s1="/storage/emmc/pic";
		char* s2=malloc(1024);
		sprintf(s2, "%d", num);//int转char*
		char* s3=".ppm";
		char *str=malloc(1024);
		strcpy(str, s1);
		strcat(str, s2);
		strcat(str, s3);
		FILE *fp = fopen(str, "wb");//"/storage/emmc/bmp.ppm"
		if(NULL == fp){
			printf("file open error %s\n", "fileerror");
		}
		//LOGE("len---->%s","got_picture");
		fprintf(fp, "P6\n%d %d\n255\n", pCodecCtx->width, pCodecCtx->height);
		// write pixel data
		int y=0;
		for(y = 0; y < pCodecCtx->height; y++){
			fwrite(pFrameRGB->data[0] + y * pFrameRGB->linesize[0], 1, pCodecCtx->width * 3, fp);
		}
		fclose(fp);
		sws_freeContext(img_convert_ctx);
		free(s2);
		free(str);
	}
	//av_free_packet(&pkt);
	av_free(avframe);
	av_free(pFrameRGB);
	(*env)->ReleaseByteArrayElements(env, framedata, pktdata, 0);
	//LOGE("len---->%i::::%i--->%i=====>%i",len,got_picture,avframe->pict_type,avframe->key_frame);
	num=num+1;
}







