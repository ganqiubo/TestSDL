package com.cc.socket.main;

import java.util.List;

import org.libsdl.app.R;

import com.cc.encoder.EncodingFrames;
import com.cc.socket.dao.EncFraThread;
import com.cc.socket.entity.PreSize;
import com.cc.socket.util.SupportPreSize;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.widget.RelativeLayout.LayoutParams;
import android.view.SurfaceHolder.Callback;

public class SenderActivity extends Activity {

	private SurfaceView mSurfaceview;
	private Camera camera;
	private SurfaceHolder holder;
	public static int preW,preH;
	private PreSize MyPreSize;
	private SupportPreSize supPreSize=new SupportPreSize();
	public EncFraThread encFraThread;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_sender);
		
		mSurfaceview = (SurfaceView) this.findViewById(R.id.sur_frames);
		if (camera==null) {
			 camera=Camera.open();
		}
		encFraThread=new EncFraThread();
		encFraThread.start();
		MyPreSize=supPreSize.getNearSize(camera,300,200);
		setSurfaceWH(MyPreSize);
		holder=mSurfaceview.getHolder();
		holder.addCallback(new callback());
		
		
	}

	public class callback implements Callback {

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			// TODO Auto-generated method stub
			try {
				if (camera==null) {
					 camera=Camera.open();
				}
				camera.setPreviewDisplay(holder);
				camera.setPreviewCallback(new preCallBack());
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	        // 开始预览
			Log.e("", camera+"----->surfaceCreated");
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			// TODO Auto-generated method stub
			Log.e("", camera+"----->surfaceChanged");
			try {
            	Parameters parameters = camera.getParameters();            	
    		
            	int maxfps=0,minfps=0;
            	List<int[]> range=parameters.getSupportedPreviewFpsRange();
            	Log.e("", "range:"+range.size());
            	for(int j=0;j<range.size();j++) {
	            	int[] r=range.get(j);
	            	for(int k=0;k<r.length;k++) {
	            		minfps=r[0];
	            		maxfps=r[1];
	            	}
            	}
            	Log.e("", minfps+"rangemax:"+maxfps);
            	//parameters.setPreviewFpsRange(15000, 15000);
            	parameters.setPreviewFormat(ImageFormat.NV21);
    			parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);//1连续对 
                if (MyPreSize.width != 0) {
                	parameters.setPreviewSize(MyPreSize.width, MyPreSize.height);
                    Size s = parameters.getPreviewSize();
                    Log.e("", MyPreSize.width+"::" +s.width+ "o^o" + s.height+"::"+MyPreSize.height);
                    preW=MyPreSize.width;preH=MyPreSize.height;
                }
                camera.setParameters(parameters);
                camera.startPreview();
    			//camera.setDisplayOrientation(90);
            } catch (Exception e) {
                e.printStackTrace();
            }
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			// TODO Auto-generated method stub
			if (camera != null) {
                // 7.结束程序时，调用Camera的StopPriview()结束取景预览，并调用release()方法释放资源.
				camera.setPreviewCallback(null);
                camera.stopPreview();
                camera.release();
                camera = null;
                Log.e("", "----->surfaceDestroyed");
            }
		}
		
	}
	
	class preCallBack implements PreviewCallback{
		@Override
		public void onPreviewFrame(byte[] data, Camera camera) {
			// TODO Auto-generated method stub
			if (encFraThread.rawDatas.size()==0 && MainActivity.ss.sendFramNum<=20) {
				encFraThread.rawDatas.add(data);
				//Log.e("", "<<<<<<<<<<<<SenderActivity>>>>>>>>>"+MainActivity.ss.sendFramNum);
			}
		}
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();

	}
	
	public void setSurfaceWH(PreSize preSize) {
		LayoutParams layoutParams=(LayoutParams) mSurfaceview.getLayoutParams();
		layoutParams.width=preSize.width;
		layoutParams.height=preSize.height;
	}
	
}
