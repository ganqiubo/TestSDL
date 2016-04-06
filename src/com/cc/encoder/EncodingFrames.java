package com.cc.encoder;

import android.util.Log;

public class EncodingFrames{
	
	public native void frameEncodeInit(int w,int h,String fileLog);
	
	public native byte[] encodcframe(byte[] data);

	static{
		System.loadLibrary("avutil-54");
    	System.loadLibrary("swresample-1");
    	System.loadLibrary("avcodec-56");
    	System.loadLibrary("swscale-3");
    	System.loadLibrary("avformat-56");
    	System.loadLibrary("avfilter-5");
    	System.loadLibrary("avdevice-56");
        System.loadLibrary("SDL2");
		System.loadLibrary("encodcframes");
		Log.e("", "====>loadLibrary");
	}
}