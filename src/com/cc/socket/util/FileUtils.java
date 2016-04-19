package com.cc.socket.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.util.Log;

public class FileUtils {
	
	 public static String readFile(String filePath) {
	        String fileContent = "";
	        File file = new File(filePath);
	        if (file == null || !file.isFile()) {
	            return null;
	        }
	 
	        BufferedReader reader = null;
	        try {
	            InputStreamReader is = new InputStreamReader(new FileInputStream(file));
	            reader = new BufferedReader(is);
	            String line = null;
	            int i = 0;
	            while ((line = reader.readLine()) != null) {
	                fileContent += line + " ";
	            }
	            reader.close();
	            return fileContent;
	        } catch (IOException e) {
	            e.printStackTrace();
	        } finally {
	            if (reader != null) {
	                try {
	                    reader.close();
	                } catch (IOException e) {
	                    e.printStackTrace();
	                }
	            }
	        }
	        return fileContent;
	    }
	
	 public static ArrayList<File> getrootFiles() {
		 
		 ArrayList<File> files=new ArrayList<File>();
		 String[] toSearch = FileUtils.readFile("/etc/vold.fstab").split(" ");
	        ArrayList<String> out = new ArrayList<String>();
	        for (int i = 0; i < toSearch.length; i++) {
	            if (toSearch[i].contains("dev_mount")) {
	                if (new File(toSearch[i + 2]).exists()) {
	                    out.add(toSearch[i + 2]);
	                    System.out.println("---->"+toSearch[i + 2]);
	                    File file = new File(toSearch[i + 2]);
	                    if (file.exists() && file.isDirectory() && file.canWrite())  {
	                    	files.add(file);
						}
	                    Log.e("", "---->"+toSearch[i + 2]);
	                }
	            }
	        }      
		 
		return files;
	 }
	 
	
	 
}
