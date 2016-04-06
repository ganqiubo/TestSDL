package com.cc.encoding;

import java.util.ArrayList;
import java.util.List;
import org.libsdl.app.R;
import com.cc.decodec.DecodecFrames;
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
import android.view.SurfaceHolder.Callback;

public class EncodingFrames extends Activity {

	private SurfaceView mSurfaceview;
	private Camera camera;
	private SurfaceHolder holder;
	private EncodcFrame encodcFrame;
	public static int preW,preH;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_encoding_frames);
		
		encodcFrame=new EncodcFrame();
		encodcFrame.start();
		mSurfaceview = (SurfaceView) this.findViewById(R.id.sur_frames);
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
				encodcFrame.camera=camera;
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
                    preW=preWidth;preH=preHeight;
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
			if (encodcFrame.rowData.size()==0) {
				encodcFrame.rowData.add(data);
			}
		}
	}
	
}


/////////////////////////////////////////////////////////////////////////////
class EncodcFrame extends Thread{
	
	public ArrayList<byte[]> rowData=new ArrayList<byte[]>();
	public static int sc=0;
	public boolean cancer=false;
	public Camera camera;
	public DecodcFrameThread dft=new DecodcFrameThread();	
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		while(!cancer){
			if (rowData.size()>0) {
				byte[] data = rowData.get(0);
				if (sc==0) {
					Size size = camera.getParameters().getPreviewSize();
					frameEncodeInit(size.width,size.height,"/storage/emmc/log1.txt");
					dft.w=size.width;
					dft.h=size.height;
					dft.start();
					//frameEncodeInit(size.width,size.height,"/storage/sdcard0/src01.flv","/storage/sdcard0/log.txt");
				}if (sc>0 && sc<=200) {
					long time = System.currentTimeMillis();
					byte[] bs=encodcframe(data);
					//if (dft.pktData.size()==0) {
					dft.pktData.add(bs);
					//}
					//Log.e("", bs.length+"<<<<<<<ds----->"+(System.currentTimeMillis()-time));
				}if (sc==200) {
					cancer=true;
					Log.e("", "EncodcFrame----->end");
				}
				rowData.remove(data);
				sc=sc+1;
			}
		}	
	}
	
	public native void frameEncodeInit(int w,int h,String fileLog);
	
	public native byte[] encodcframe(byte[] data);
	
	static{
		System.loadLibrary("avutil-54");
    	System.loadLibrary("swresample-1");
    	System.loadLibrary("avcodec-56");
    	System.loadLibrary("swscale-3");
    	System.loadLibrary("avformat-56");
    	System.loadLibrary("avfilter-5");
    	System.loadLibrary("avdevice-56");
        System.loadLibrary("SDL2");
		System.loadLibrary("encodcframes");
		Log.e("", "====>loadLibrary");
	}
}

class DecodcFrameThread extends Thread{
	public ArrayList<byte[]> pktData=new ArrayList<byte[]>();
	public boolean cancer=false;
	public DecodecFrames df=new DecodecFrames();
	public boolean isfirst=true;
	public int w,h; 

	@Override
	public void run() {
		// TODO Auto-generated method stub
		
		while(!cancer){
			if (pktData.size()>0) {
				byte[] pktdata=pktData.get(0);
				if (isfirst) {
					df.decodecFrameInit(w, h, "/storage/emmc/log2.txt");
					isfirst=false;
				}if (!isfirst) {
					df.decodecFrame(pktdata);
				}if (EncodcFrame.sc>=200) {
					cancer=true;
				}
				pktData.remove(0);
			}
		}
	}
	
}
