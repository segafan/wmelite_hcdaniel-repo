package org.deadcode.wmelite;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Environment;

public class WMELiteFunctions {

	private Context c;
	
	public WMELiteFunctions() {
	}
	
	public void setContext(Context c) {
		this.c = c;
	}
	
	public native void nativeInit();
	
	public String getLogFileDirectory() {
		// this assumed the "external" storage exists and is mounted r/w
		// return Environment.getExternalStorageDirectory().getAbsolutePath();
		
		return "/mnt/sdcard/";
	}
	
	public String getPrivateFilesPath() {
		// this returns the app-private storage for user data
		return c.getFilesDir().getAbsolutePath();
	}

	public String getDeviceTypeHint() {
		if (c != null) {
			return ((c.getResources().getConfiguration().screenLayout
	            & Configuration.SCREENLAYOUT_SIZE_MASK)
	            >= Configuration.SCREENLAYOUT_SIZE_LARGE) ? "tablet" : "phone";
		} else {
			return "tablet";
		}
	}

	public String getGamePackagePath() {
		// change to fit your needs, maybe to point to the expansion files downloaded from Google play
		// return Environment.getExternalStorageDirectory().getAbsolutePath();
		
		return "/mnt/sdcard/";
	}

	public String getGameFilePath() {
		// change to fit your needs, maybe to point to the expansion files downloaded from Google play
		// return Environment.getExternalStorageDirectory().getAbsolutePath();
		
		return "/mnt/sdcard/";
	}

	public String getFontPath() {
		if (c != null) {
			// FIXME is this correct?
			return c.getPackageResourcePath() + "/assets/files/";
		} else {
			return ".";
		}
	}
}
