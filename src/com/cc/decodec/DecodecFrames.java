package com.cc.decodec;

import android.util.Log;

public class DecodecFrames {

	static{
		System.loadLibrary("avutil-54");
    	System.loadLibrary("swresample-1");
    	System.loadLibrary("avcodec-56");
    	System.loadLibrary("swscale-3");
    	System.loadLibrary("avformat-56");
    	System.loadLibrary("avfilter-5");
    	System.loadLibrary("avdevice-56");
        System.loadLibrary("SDL2");
		System.loadLibrary("decodeframes");
		
		Log.e("", "====>loadLibrary");
	}
	
	
	
	public DecodecFrames() {
		// TODO Auto-generated constructor stub
	}

	public native void decodecFrameInit(int w,int h,String fileLog);
	
	public native void decodecFrame(byte[] pktData);
	
}
