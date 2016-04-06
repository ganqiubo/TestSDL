package com.cc.socket.main;

import java.util.ArrayList;
import java.util.List;

import org.libsdl.app.R;
import android.app.Activity;
import android.os.Bundle;

public class ReceiverActivity extends Activity {

	private static List<Byte> recePktData=new ArrayList<Byte>();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_receiver);
		
		
		
	}

}
