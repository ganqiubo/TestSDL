package com.cc.socket.dao;

import java.io.ByteArrayInputStream;
import java.io.ObjectInputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.ArrayList;
import java.util.List;

import com.cc.socket.entity.Device;
import com.cc.socket.entity.SocketData;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class ReceiveSocket extends Thread{

	public DatagramSocket datagramSocket;
	public List<SocketData> socketDatas;
	public Device receiver;
	public boolean cancer=false;
	public Info info;
	public DecFraThread decFraThread;
	
	
	public ReceiveSocket(Device receiver,ReceiveSocket.Info info) {
		this.receiver = receiver;
		this.socketDatas=new ArrayList<SocketData>();
		this.info=info;
		try {
			datagramSocket=new DatagramSocket(receiver.port);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			Log.e("", "ReceiveSocket>>>>>>>>>>"+e);
		}
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		Log.e("", datagramSocket+">>>>>>>>>>wait..."+datagramSocket.getLocalSocketAddress());
		while(!cancer){
			DatagramPacket pack = null;
			ByteArrayInputStream bais = null;
			ObjectInputStream ois = null;
			try {
				pack=new DatagramPacket(new byte[1024*10], 1024*10);
				datagramSocket.receive(pack);
				bais=new ByteArrayInputStream(pack.getData());
	            ois=new ObjectInputStream(bais);
	            SocketData socketData = (SocketData)ois.readObject();
	            socketData.remoteAddress=pack.getAddress().getHostAddress();
	            Message message=new Message();
	            message.obj=socketData;
	            handler.sendMessage(message);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				Log.e("", "ReceiveSocketrun>>>>>>>>>>"+e);
			}
		}
		if (datagramSocket!=null) {
			datagramSocket.close();
		}
		Log.e("", "receivefinish>>>>>>>>>>");
	}

	private Handler handler=new Handler(){
		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			SocketData socketData=(SocketData) msg.obj;
			//if (socketData.flag!=2) {
			info.sendmessage(socketData);
			//}
			if (socketData.flag==2) {
				if (decFraThread==null) {
					decFraThread=new DecFraThread();
					decFraThread.videoW=socketData.videoW;
					decFraThread.videoH=socketData.videoH;
					decFraThread.start();
				}
				decFraThread.pktDatas.add(socketData.pktdata);
			}
		}
	};
	
	public interface Info{
		public void sendmessage(SocketData socketData);
	}
	
}
