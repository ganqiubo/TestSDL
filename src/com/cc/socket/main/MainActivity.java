package com.cc.socket.main;

import java.net.InetSocketAddress;

import org.libsdl.app.R;

import com.cc.socket.dao.ReceiveSocket;
import com.cc.socket.dao.SendSocket;
import com.cc.socket.entity.Device;
import com.cc.socket.entity.SocketData;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends Activity {

	private Button send;
	private EditText ip_et,port_et,message;
	private String LocalWifiIp;
	private Device receiver;
	public static ReceiveSocket rs;
	public static SendSocket ss;
	private Context mContext;
	public static int localPort=9908;
	private long time=0;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main_socket);
		
		mContext=this;
		send=(Button) findViewById(R.id.send);
		ip_et=(EditText) findViewById(R.id.ip_et);
		port_et=(EditText) findViewById(R.id.port_et);
		message=(EditText) findViewById(R.id.message);
				
		send.setOnClickListener(new btclick());
		
		receRegister();
		sendRegister();
		
	}
	
	class btclick implements OnClickListener{

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			sendInfo(new SocketData(0, message.getText().toString()),
					new InetSocketAddress(ip_et.getText().toString(),Integer.parseInt(port_et.getText().toString().trim())));
		}
	}
	
	private void sendInfo(final SocketData socketData, InetSocketAddress inetSocketAddress) {
		// TODO Auto-generated method stub
		ss.inetSocketAddress=inetSocketAddress;
		socketData.remotePort=localPort;
		new Thread(new Runnable() {
			@Override
			public void run() {
				// TODO Auto-generated method stub
				ss.senddata(socketData);
			}
		}).start();
		
	}
	
	private void sendRegister() {
		// TODO Auto-generated method stub
		ss=new SendSocket(new InetSocketAddress(ip_et.getText().toString(),Integer.parseInt(port_et.getText().toString().trim())),
			new ReceiveSocket.Info() {
			   @Override
			   public void sendmessage(SocketData socketData) {
				   // TODO Auto-generated method stub
					
			   }
			});	
		Log.e("", "onsend111------->");
	}

	
	private void receRegister() {
		// TODO Auto-generated method stub
		receiver=new Device(LocalWifiIp, localPort);
		rs=new ReceiveSocket(receiver,new ReceiveSocket.Info() {
			@Override
			public void sendmessage(SocketData socketData) {
				// TODO Auto-generated method stub
				if(socketData.flag==0){
					flag0(socketData);
				}if (socketData.flag==1) {
					flag1(socketData);
				}if (socketData.flag==2) {
					Toast.makeText(mContext, (System.currentTimeMillis()-time)+
							"-receive-->"+socketData.videoW+"::"+socketData.videoH+
							"--->"+socketData.pktdata.length, Toast.LENGTH_SHORT).show();
					Log.e("", (System.currentTimeMillis()-time)+
							"-receive-->"+socketData.videoW+"::"+socketData.videoH+
							"--->"+socketData.pktdata.length);
					time=System.currentTimeMillis();
				}
				
			}
		});
		rs.start();
	}
	
	public void flag0(SocketData socketData) {
		Toast.makeText(mContext, socketData.remoteAddress+
				"------>"+socketData.header, Toast.LENGTH_SHORT).show();
		
		SocketData socketdata=new SocketData(1, "I am ready");
		InetSocketAddress inetSocketAddress=new InetSocketAddress(socketData.remoteAddress,socketData.remotePort);
		sendInfo(socketdata,inetSocketAddress);
		sleep(500);
		Intent intent=new Intent(MainActivity.this,ReceiverActivity.class);
		startActivity(intent);
		//this.finish();
	}
	
	public void flag1(SocketData socketData) {
		Toast.makeText(mContext, socketData.remoteAddress+
				"------>"+socketData.header, Toast.LENGTH_SHORT).show();
		sleep(500);
		Intent intent=new Intent(MainActivity.this,SenderActivity.class);
		startActivity(intent);
	}
	
	public void sleep(long time){
		try {
			Thread.sleep(time);
		} catch (Exception e) {
			// TODO Auto-generated catch block
		}
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		rs.cancer=true;
		rs.datagramSocket.close();
		ss.datagramSocket.close();
	}

}
