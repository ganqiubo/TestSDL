package com.cc.socket.util;

import java.util.List;

import com.cc.socket.entity.PreSize;

import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.util.Log;

public class SupportPreSize {

	public PreSize getMatchScaleSize(Camera camera,int width, int height){
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
        return new PreSize(preWidth, preHeight);
	}
	
	public PreSize getNearSize(Camera camera,int width, int height){
		PreSize preSize=new PreSize();
		List<Size> previewSizes = camera.getParameters().getSupportedPreviewSizes();
		int ds=Integer.MAX_VALUE;
		for (int i = 0, len = previewSizes.size(); i < len; i++) {
            Size s = previewSizes.get(i);
            Log.e("", s.width+"::"+s.height);
            if (Math.abs((s.width+s.height-width-height))<ds) {
            	preSize.width=s.width;
            	preSize.height=s.height;
            	ds=Math.abs((s.width+s.height-width-height));
			}
		}    
		return preSize;
	}
	
	public PreSize getMinSize(Camera camera,int width, int height){
		PreSize preSize=new PreSize(width, height);
		List<Size> previewSizes = camera.getParameters().getSupportedPreviewSizes();
        int total=preSize.width+preSize.height;
        for (int i = 0, len = previewSizes.size(); i < len; i++) {
            Size s = previewSizes.get(i);
            Log.e("", s.width+"::"+s.height);
            if ((s.width+s.height)<total) {
            	preSize.width = s.width;
            	preSize.height = s.height;
                total=preSize.width+preSize.height;
            }
        }
        return preSize;
	}
	
}
