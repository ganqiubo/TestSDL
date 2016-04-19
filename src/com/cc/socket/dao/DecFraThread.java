package com.cc.socket.dao;

import java.util.ArrayList;
import java.util.List;
import com.cc.decodec.DecodecFrames;

import android.util.Log;

public class DecFraThread extends Thread{

	public List<byte[]> pktDatas=new ArrayList<byte[]>();
	public boolean cancer=false;
	public DecodecFrames decFras;
	public int videoW,videoH;
	public long time;
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		super.run();
		while (!cancer) {
			if (pktDatas.size()>0) {
				if (decFras==null) {
					decFras=new DecodecFrames();
					decFras.decodecFrameInit(videoW, videoH, "/storage/sdcard1/log3.txt");
				}
				time=System.currentTimeMillis();
				decFras.decodecFrame(pktDatas.get(0));
				Log.e("", "decFras>>>>>>>>"+(System.currentTimeMillis()-time));
				pktDatas.remove(0);
			}
		}
	} 
	
}
