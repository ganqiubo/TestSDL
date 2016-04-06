package com.cc.socket.entity;

import java.io.Serializable;
import java.net.InetAddress;

public class SocketData implements Serializable{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public int flag;
	
	public String header;
	
	public byte[] pktdata;

	
	
	public SocketData() {
		// TODO Auto-generated constructor stub
	}

	public SocketData(int flag, String header) {
		this.flag = flag;
		this.header = header;
	}

	//set by receiver
	public InetAddress localAddress;
	
	//set by sender
	public String remoteAddress;
	
	public int remotePort;
	
	public int videoW;
	
	public int videoH;
}
