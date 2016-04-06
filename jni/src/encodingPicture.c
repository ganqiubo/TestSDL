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

#define LOG_TAG "GQB_encoding"
#undef LOG
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
int hh=1;

jstring Java_com_cc_encoding_FfmEncodingActivity_encod(JNIEnv* env,jobject thiz){

	LOGE( "SDL_CreateWindow-----> %s", "kkkk");

	return (*env)->NewStringUTF(env, "oh");
}

void Java_com_cc_encoding_FfmEncodingActivity_frameEncode(JNIEnv* env,jobject thiz,jbyteArray framedata,jint w,jint h,jstring file){
	int i, sum = 0;
	//int w=width,h=height;
	int len = (*env)->GetArrayLength(env, framedata);
	LOGE( "%i===%i::len-----> %i",w,h,len);

	uint8_t *data=(*env)->GetByteArrayElements(env, framedata, 0);

	AVFrame *pFrameRGB,*pFrameYUV;
	pFrameRGB =av_frame_alloc();//分配内存
	pFrameYUV =av_frame_alloc();//分配内存

	uint8_t *pBuffer,*pBuffer1;
	int numBytes,numBytes1;
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, w, h);
	pBuffer=(uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameRGB, pBuffer, AV_PIX_FMT_RGB24,w, h);

	numBytes1 = avpicture_get_size(AV_PIX_FMT_YUV420P, w, h);
	avpicture_fill((AVPicture *)pFrameYUV, data, AV_PIX_FMT_NV21,w, h);
	LOGE( "pFrameYUV-----> %i",numBytes);
	struct SwsContext *img_convert_ctx;

	LOGE( "11111111111111%s","ii");
	img_convert_ctx = sws_getContext(
			w, h, AV_PIX_FMT_NV21,
			w, h,
			AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	LOGE( "222222222222222%s","oo");
	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrameYUV->data, pFrameYUV->linesize, 0,
			h, pFrameRGB->data, pFrameRGB->linesize);
	LOGE( "333333333333333%s","oo");
	const char *str = (*env)->GetStringUTFChars(env, file, 0);
	LOGE("file----->%s", str);

	FILE *fp = fopen(str, "wb");//"/storage/emmc/bmp.ppm"
	if(NULL == fp){
		printf("file open error %s\n", "fileerror");
	}
	fprintf(fp, "P6\n%d %d\n255\n", w, h);
	// write pixel data
	int y=0;
	for(y = 0; y < h; y++){
		fwrite(pFrameRGB->data[0] + y * pFrameRGB->linesize[0], 1, w * 3, fp);
	}

	fclose(fp);
	sws_freeContext(img_convert_ctx);
	(*env)->ReleaseByteArrayElements(env, framedata, data, 0);
	(*env)->ReleaseStringUTFChars(env, file, str);
}
/*
void Java_com_cc_encoding_FfmEncodingActivity_frameParam(JNIEnv* env,jobject thiz,jint w,jint h){

	//int width=(*enc)->Get
	LOGE( "SDL_CreateWindow-----> %i", w);
	width=w;
	height=h;

}*/
