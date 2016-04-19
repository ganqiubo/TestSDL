package com.cc.socket.util;

import java.io.File;
import android.os.Environment;
import android.util.Log;

public class GetRootFloder {

	public static String FloderName(){
		String floderName="";
		File f=new File(Environment.getExternalStorageDirectory()+"/gqb");
		f.mkdir();
		if(!f.exists()){
    		if (f.mkdir()) {
    			floderName=Environment.getExternalStorageDirectory()+"";
    			//dboutput(f1.getPath());
			}else{
				for (File fn : FileUtils.getrootFiles()) {
					if (!fn.getPath().equals(Environment.getExternalStorageDirectory().getPath())) {
						f=new File(fn.getPath()+"/gqb");
						f.mkdir();
						if (f.exists()) {
							floderName=fn.getPath();
						}
						Log.e("", "@=@1-->"+fn.getPath());
						break;
					}
				}
			}
    	}
		return floderName;
	}
}
