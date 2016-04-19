#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>
#include <string.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>

#include "SDL.h"
#include "SDL_main.h"
#include <jni.h>
#define LOG_TAG "GQB"
#undef LOG
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj,jint mtype,jint mc)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
	SDL_Android_Init(env, cls);

    SDL_SetMainReady();

    av_register_all();
    int status;
    char *argv[2];
    argv[0] = SDL_strdup("SDL_app");
    argv[1] = NULL;
    LOGE("type-->>%i>>%i",mtype,mc);
    if(1==mtype){
    	playFileVideo();
    }if(2==mtype){
    	playFrameVideo();
    }
}

int playFrameVideo(){

}

int playFileVideo(){
		//const char    *url = "/storage/emmc/src01.mp4";
		//const char    *url = "/storage/sdcard0/src01.mp4";
		//const char    *url = "/storage/emmc/src01.flv";
		//const char    *url = "/storage/emmc/9934.mp4";
		//const char    *url = "/storage/emulated/0/1080p.mov";
		const char    *url = "/storage/sdcard0/9934.mp4";
		AVFormatContext *s = NULL;//视频format上下文
		AVCodecContext *avCodecContext=NULL;//
		AVCodec *avcodec=NULL;
		FILE *fp=NULL;
		int videoindex=-1;//视频流下标
		int audioindex=-1;//音频流下标
		int subtitleindex=-1;//文字流下标
		int i=0;



		int ret = avformat_open_input(&s, url, NULL, NULL);

		/*
		*d，lx，ld，，lu，这几个都是输出32位
		* hd，hx，hu，这几个都是输出16位数据
		* hhd，hhx，hhu，这几个都是输出8位
		* lld，ll，llu，llx，这几个都是输出64位
		*/
		LOGE("ret: %i",ret);
		if(ret<0){
			return 0;
		}

		//区分流类型下标
		for(i=0;i<s->nb_streams;i++){
			if(s->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
				videoindex=i;
			}if(s->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
				audioindex=i;
			}if(s->streams[i]->codec->codec_type==AVMEDIA_TYPE_SUBTITLE){
				subtitleindex=i;
			}
		}
		if(videoindex==-1){
			return 0;
		}
		LOGE("avformat_find_stream_info: %i\n",avformat_find_stream_info(s,NULL));
		//av_dump_format(s,0,url,0);

		avCodecContext=s->streams[videoindex]->codec;
		avcodec=avcodec_find_decoder(avCodecContext->codec_id);
		LOGE("codec_name: %s width: %i height: %i",avcodec->name,avCodecContext->width,avCodecContext->height);

		LOGE("ffmpegversion: %i\nduring: %llu\nbit_rate: %i\n%i\nname: %s",avcodec_version(),
				s->duration,s->bit_rate,s->nb_streams,avcodec->name);

		AVPacket *pkt=NULL;
		AVFrame *avframe=NULL;
		AVFrame *pFrameRGB = NULL;
		FILE *yuvstream;
		int *got_picture_ptr=NULL;
		if(avcodec_open2(avCodecContext,avcodec,NULL)<0){
			return 0;
		}
		i=0;
		pkt = (AVPacket *)av_malloc(sizeof(AVPacket));//分配内存
		avframe=av_frame_alloc();//分配内存
		pFrameRGB = av_frame_alloc();//分配内存
		got_picture_ptr=(int *)av_malloc(sizeof(int));

		uint8_t *pBuffer;
		int numBytes;
		numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, avCodecContext->width, avCodecContext->height);
		pBuffer=(uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
		avpicture_fill((AVPicture *)pFrameRGB, pBuffer, AV_PIX_FMT_RGB24,avCodecContext->width, avCodecContext->height);

		SDL_Window *window;
		SDL_Renderer *renderer;
		uint8_t *out_buffer;
		int w, h;
		LOGE("SDL_INIT_EVERYTHING---->%i",SDL_Init(SDL_INIT_EVERYTHING));
		LOGE("SDL_CreateWindowAndRenderer-----> %i", SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer));
		SDL_GetWindowSize(window,&w,&h);
		LOGE("SDL_CreateWindow-----> %s", SDL_GetError());
		if(window!=NULL){
			LOGE("window-----> %s", "!!!!!!NULL");
		}
		LOGE("window-----> %i", w);
		SDL_Texture* sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,avCodecContext->width,avCodecContext->height);
		AVFrame *pFrameYUV=av_frame_alloc();
		out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, avCodecContext->width, avCodecContext->height));
		avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, avCodecContext->width, avCodecContext->height);

		struct SwsContext *img_convert_ctx;
		img_convert_ctx = sws_getContext(
				 	avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
				 	avCodecContext->width, avCodecContext->height,
				 	PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
		while(1){
			if(av_read_frame(s, pkt)<0){//获取原始数据
				 return 0;
			}if(pkt->stream_index==videoindex){
				 avcodec_decode_video2(avCodecContext, avframe,got_picture_ptr, pkt);
				if(got_picture_ptr){
					 sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data, avframe->linesize, 0,
							avCodecContext->height, pFrameYUV->data, pFrameYUV->linesize);

					SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
					SDL_RenderClear(renderer);
										//SDL_RenderCopy( renderer, sdlTexture, &sdlRect, &sdlRect );
					SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
										//SDL_RenderCopyEx(renderer, sdlTexture, NULL,NULL, 0, NULL, SDL_FLIP_HORIZONTAL);
					SDL_RenderPresent(renderer);
					SDL_Delay(0);
				}
				av_free_packet(pkt);
				i++;
			}
		}
			sws_freeContext(img_convert_ctx);
}

