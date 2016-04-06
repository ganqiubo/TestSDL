package com.cc.encoding;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.libsdl.app.R;
import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.widget.Toast;

public class FfmEncodingFile extends Activity {

	private SurfaceView mSurfaceview;
	private Camera camera;
	private SurfaceHolder holder;
	
	private Context ct;
	private WriteFile writeFile;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_ffm_encoding);
		
		ct=this;
		File file=new File(Environment.getExternalStorageDirectory()+"/src01.flv");
		if (file.exists()) {
			file.delete();
			Log.e("", "delete======>src01.flv");		
		}		
		file=new File(Environment.getExternalStorageDirectory()+"/log.txt");
		if (file.exists()) {
			file.delete();
			Log.e("", "delete======>log.txt");		
		}
		mSurfaceview = (SurfaceView) this.findViewById(R.id.surid);
		holder=mSurfaceview.getHolder();
		holder.addCallback(new callback());
		writeFile=new WriteFile(new OnWriteFinish() {
			
			@Override
			public void onWriteFinish() {
				// TODO Auto-generated method stub
				Toast.makeText(ct, "finish", Toast.LENGTH_LONG).show();
			}
		});
		writeFile.start();
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
				writeFile.camera=camera;
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
                List<Size> previewSizes = camera.getParameters().getSupportedPreviewSizes();
                double aspectTolerance = Double.MAX_VALUE;
                int preWidth = 0, preHeight = 0;
                double scale = ((double) width) / height;
                for (int i = 0, len = previewSizes.size(); i < len; i++) {
                    Size s = previewSizes.get(i);
                    Log.e("", s.width+"::"+s.height);
                    double sizeScale = ((double) s.height) / s.width;
                    if (Math.abs(scale - sizeScale) < aspectTolerance) {
                        aspectTolerance = Math.abs(scale - sizeScale);
                        preWidth = s.width;
                        preHeight = s.height;
                    }
                }
                if (preWidth != 0) {
                    parameters.setPreviewSize(preWidth, preHeight);
                    // mSurface.setLayoutParams(new LayoutParams(720, 1280));
                    Size s = parameters.getPreviewSize();
                    Log.e("", preWidth+"::"+s.width + " " + s.height+"::"+preHeight);
                }
                camera.setParameters(parameters);
                camera.startPreview();
    			camera.setDisplayOrientation(90);
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
			//PixelFormat.YCbCr_420_SP
			//ImageFormat.NV21
			if (writeFile.rowData.size()==0) {
				writeFile.rowData.add(data);
			}
		}
	} 

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		writeFile.cancer=true;
	}
	

}

//////////////////////////////////////////////////////////////////////////////////////////////////
class WriteFile extends Thread {
	
	public ArrayList<byte[]> rowData=new ArrayList<byte[]>();
	public int sc=0;
	public boolean cancer=false;
	public Camera camera;
	private long time;
	private OnWriteFinish onWriteFinish;
	
	public WriteFile(OnWriteFinish onWriteFinish) {
		this.onWriteFinish = onWriteFinish;
	}

	public void run() {
		while(!cancer){
			if (rowData.size()>0) {
				byte[] data = rowData.get(0);
				if (sc==0) {
					Size size = camera.getParameters().getPreviewSize();
					frameEncodeInit(size.width,size.height,"/storage/emmc/src01.flv","/storage/emmc/log.txt");
					//frameEncodeInit(size.width,size.height,"/storage/sdcard0/src01.flv","/storage/sdcard0/log.txt");
				}if (sc>0 && sc<=200) {
					long time = System.currentTimeMillis();
					byte[] bs=Writeframe(data);
					Log.e("", bs.length+"<<<<<<<ds----->"+(System.currentTimeMillis()-time));
				}if (sc==200) {
					WriteFinish();
					cancer=true;
					Message msg=new Message();
					msg.what=0;
					handler.sendMessage(msg);
				}
				rowData.remove(data);
				sc=sc+1;
			}
		}	
	}
	
	private Handler handler = new Handler() {
       @Override
       public void handleMessage(Message msg) {
    	// TODO Auto-generated method stub
    	   super.handleMessage(msg);
    	   switch (msg.what) {
           case 0:
        	   onWriteFinish.onWriteFinish();
	           break;
    	   }
       }
    };
	
	public native void frameEncodeInit(int w,int h,String file,String fileLog);
	
	public native byte[] Writeframe(byte[] data);
	
	public native void WriteFinish();
	
	static{
		System.loadLibrary("avutil-54");
    	System.loadLibrary("swresample-1");
    	System.loadLibrary("avcodec-56");
    	System.loadLibrary("swscale-3");
    	System.loadLibrary("avformat-56");
    	System.loadLibrary("avfilter-5");
    	System.loadLibrary("avdevice-56");
        System.loadLibrary("SDL2");
		System.loadLibrary("encodingfile");
		Log.e("", "====>loadLibrary");
	}
	
}

///////////////////////////////////////////////////////////////////////////////////
interface OnWriteFinish{
	public void onWriteFinish();
}

