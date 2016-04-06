package com.cc.socket.dao;

import java.util.ArrayList;
import java.util.List;
import com.cc.encoder.EncodingFrames;
import com.cc.socket.entity.SocketData;
import com.cc.socket.main.MainActivity;
import com.cc.socket.main.SenderActivity;
import android.util.Log;

public class EncFraThread extends Thread{

	public List<byte[]> rawDatas=new ArrayList<byte[]>();
	public boolean cancer=false;
	public EncodingFrames encFras; 
	public long time;
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		super.run();
		while(!false) {
			time=System.currentTimeMillis();
			if (rawDatas.size()>0) {
				if (encFras==null) {
					encFras=new EncodingFrames();
					encFras.frameEncodeInit(SenderActivity.preW, SenderActivity.preH, "/storage/emmc/log2.txt");
				}
				byte[] pktData=encFras.encodcframe(rawDatas.get(0));
				
				time=System.currentTimeMillis();
				SocketData socketData=new SocketData(2, "pktdata");
				socketData.pktdata=pktData;
				socketData.videoW=SenderActivity.preW;
				socketData.videoH=SenderActivity.preH;
				//decFrams.decodecFrame(pktData);
				MainActivity.ss.senddata(socketData);
				Log.e("", pktData.length+"---->"+(System.currentTimeMillis()-time));
				rawDatas.remove(0);
			}
		}
	}
	
}
