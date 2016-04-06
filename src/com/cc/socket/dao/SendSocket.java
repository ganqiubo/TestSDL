package com.cc.socket.dao;

import java.io.ByteArrayOutputStream;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.List;

import com.cc.socket.entity.SocketData;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class SendSocket{

	public DatagramSocket datagramSocket;
	public InetSocketAddress inetSocketAddress;
	public ReceiveSocket.Info info;
	public int sendFramNum=0;
	public int packSize=1024 * 10;
	public long time;
	
	public SendSocket(InetSocketAddress inetSocketAddress,ReceiveSocket.Info info) {
		this.inetSocketAddress=inetSocketAddress;
		this.info=info;
		try {
			this.datagramSocket=new DatagramSocket();
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			Log.e("", "e<<<{o-o}>>>"+e);
		}
	}

	public void senddata(SocketData socketData) {
		// TODO Auto-generated method stub
		ByteArrayOutputStream bos = null;
		ObjectOutputStream oos = null;
		try {
			bos=new ByteArrayOutputStream();
			oos=new ObjectOutputStream(bos);
			oos.writeObject(socketData);

			byte [] b=bos.toByteArray();

			DatagramPacket pack=new DatagramPacket(b, b.length,inetSocketAddress);
			//time=System.currentTimeMillis();
			datagramSocket.send(pack);
			if (socketData.flag==2) {
				sendFramNum++;
			}
			time=System.currentTimeMillis();
			oos.flush();
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			Log.e("", "e1------>"+e1);
			e1.printStackTrace();
		}
	}

	private Handler handler=new Handler(){
		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			
		}
	};
	
}
