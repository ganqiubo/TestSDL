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

#define LOG_TAG "GQB"
#undef LOG
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

int main(int argc, char *argv[])
{
	/*int i=0;
	SDL_Renderer *renderer;

    LOGE("%s","mymain");
    LOGE("SDL_INIT_TIMER---->%i",SDL_Init(SDL_INIT_TIMER));
    LOGE("SDL_INIT_AUDIO---->%i",SDL_Init(SDL_INIT_AUDIO));
    LOGE("SDL_INIT_VIDEO---->%i",SDL_Init(SDL_INIT_VIDEO));

    LOGE("SDL_INIT_EVERYTHING---->%i",SDL_Init(SDL_INIT_EVERYTHING));

    LOGE("SDL_GetNumVideoDrivers---->%i",SDL_GetNumVideoDrivers());
    LOGE("SDL_GetNumVideoDisplays---->%i",SDL_GetNumVideoDisplays());


    SDL_Window *window=SDL_GL_GetCurrentWindow();


    if(window!=NULL){
    	LOGE("%s","!!!!!!!!!!!!!!null");
    	LOGE( "SDL_CreateWindow-----> %s", SDL_GetError());
    }if(window==NULL){
    	LOGE("%s","nullllllll");
    }


    window = SDL_CreateWindow("My Player Window",
    		SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
    		100, 100,
    		SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    if(window==NULL){
        	LOGE("%s","nullllllllllll");
        	LOGE( "SDL_CreateWindow-----> %s", SDL_GetError());
    }
    //screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
    //LOGE( "SDL_CreateWindow-----> %s", SDL_GetError());

    SDL_Delay(8000);



    while(i<2000000){
    	i++;
    }*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		av_register_all();
		const char    *url = "/storage/emmc/9934.mp4";
		//const char    *url = "/storage/sdcard1/9934.mp4";
		AVFormatContext *s;//视频format上下文
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

			 struct SwsContext *img_convert_ctx;

			 while(i<1000){
			 		 if(av_read_frame(s, pkt)<0){//获取原始数据
			 		 		return 0;
			 		 }if(pkt->stream_index==videoindex){
			 			 avcodec_decode_video2(avCodecContext, avframe,got_picture_ptr, pkt);

			 			 if(got_picture_ptr){

			 			 }
			 			char *pPicFile="/storage/sdcard1/9934.mp4";
			 			 char szPicFile[256];
			 			 memset(szPicFile, 0, sizeof(szPicFile));
			 			 strncpy(szPicFile, pPicFile, sizeof(szPicFile));
			 			 strncat(szPicFile, ".ppm", sizeof(szPicFile));
			 			 // Convert the image from its native format to RGB
			 			 img_convert_ctx = sws_getContext(
			 					 avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
			 					 avCodecContext->width, avCodecContext->height,
			 					 AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
			 			 sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data, avframe->linesize, 0,
			 					 avCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
			 			 //SaveFrameToPPM(szPicFile, pFrameRGB,pCodecCtxDec->width, pCodecCtxDec->height);

			 			 int nWidth=avCodecContext->width;
			 			 int nHeight=avCodecContext->height;
			 			 if(i==998){
			 				 FILE *fp = fopen("/storage/sdcard1/bmp1.ppm", "wb");
			 				 if(NULL == fp){
			 				 	printf("file open error %s\n", pPicFile);
			 				 		return 0;
			 				 	}
			 				 	// write header
			 				 	fprintf(fp, "P6\n%d %d\n255\n", nWidth, nHeight);

			 				 	// write pixel data
			 				 	int y=0;
			 				 	for(y = 0; y < nHeight; y++)
			 				 	{
			 				 		fwrite(pFrameRGB->data[0] + y * pFrameRGB->linesize[0], 1, nWidth * 3, fp);
			 				 	}
			 				 	fclose(fp);
			 			 }

			 			 sws_freeContext(img_convert_ctx);
			 			 i++;
			 		 }
			 		 //LOGE("",coded_picture_number);

			 	 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	SDL_Window *window;
	SDL_Renderer *renderer;
	int w, h;
	LOGE("SDL_INIT_EVERYTHING---->%i",SDL_Init(SDL_INIT_EVERYTHING));
	LOGE("SDL_CreateWindowAndRenderer-----> %i", SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer));
	SDL_GetWindowSize(window,&w,&h);
	LOGE("SDL_CreateWindow-----> %s", SDL_GetError());
	if(window!=NULL){
		LOGE("window-----> %s", "!!!!!!NULL");
	}
	LOGE("window-----> %i", w);

	SDL_Surface* temp;
	temp = SDL_LoadBMP("/storage/sdcard1/bbm3.bmp");
	if (temp != NULL){
		LOGE("temp w:%i   h:%i", temp->w,temp->h);
	}

	struct SDL_Texture *texture;
	texture = SDL_CreateTextureFromSurface(renderer, temp);
	if (temp != NULL){
			LOGE("texture %s", "!!!!!!!!!!!null");
	}
	SDL_FreeSurface(temp);

	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);

	//renderTexture(texture, renderer, 0, 0);

	    //Update the screen
	LOGE("avcodec_configuration---->%s",avcodec_configuration());

	SDL_Rect destRect = {w/2 -temp->w/2, h/2 - temp->h/2, temp->w, temp->h};

	SDL_RenderCopy(renderer, texture, NULL, &destRect);
	//renderTexture(temp, renderer, 0, 0);
	SDL_RenderPresent(renderer);
	    //Update the screen
	//SDL_RenderPresent(renderer);

	SDL_Delay(5000);

	temp = SDL_LoadBMP("/storage/sdcard1/hu.bmp");
	texture = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_FreeSurface(temp);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &destRect);
	SDL_RenderPresent(renderer);
	SDL_Delay(5000);


}
