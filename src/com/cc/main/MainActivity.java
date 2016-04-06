package com.cc.main;

import org.libsdl.app.R;
import org.libsdl.app.SDLActivity;

import com.cc.encoding.FfmEncodingFile;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {

	private Button bt1,bt2;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		bt1=(Button) findViewById(R.id.button1);
		bt2=(Button) findViewById(R.id.button2);
		bt1.setOnClickListener(new btclick());
		bt2.setOnClickListener(new btclick());
		
	}

	class btclick implements OnClickListener {

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			Intent intent;
			switch (v.getId()) {
			case R.id.button1:
				intent=new Intent(MainActivity.this,FfmEncodingFile.class);
				startActivity(intent);
				break;
			case R.id.button2:
				intent=new Intent(MainActivity.this,SDLActivity.class);
				startActivity(intent);
				break;
			}
		}
		
	}
	
}
