package org.deadcode.wmelite;

import java.nio.charset.Charset;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;

import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.content.res.Configuration;

import android.net.Uri;

public class WMELiteFunctions {

	private Context c;
	
	public WMELiteFunctions() {
	}
	
	public void setContext(Context c) {
		this.c = c;
	}
	
	public void init() {
		nativeInit(c.getAssets());
	}
	
	private native void nativeInit(AssetManager manager);
	
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
		// return "/mnt/sdcard/demo/";
	}

	public String getGameFilePath() {
		// change to fit your needs, maybe to point to the expansion files downloaded from Google play
		// return Environment.getExternalStorageDirectory().getAbsolutePath();
		
		return "/mnt/sdcard/";
		// return "/mnt/sdcard/demo/";
	}

	public String getFontPath() {
		if (c != null) {
			// FIXME is this correct?
			return c.getPackageResourcePath() + "/assets/files/";
		} else {
			return ".";
		}
	}
	
	public String getLocalSettingsPath() {
		// for development and debugging, a local settings.xml
		// can be placed onto the device using a location
		// that can be accessed by both wmelite and the user
		
		return "/mnt/sdcard/";
	}
	
	public byte[] getEncodedString(String inputString, String charsetName) {
		
		// change the default here if all the strings used in the project have the same encoding
		Charset charset = Charset.forName("US-ASCII");
		
		if (charsetName != null) {
			try {
				charset = Charset.forName(charsetName);
			} catch (IllegalCharsetNameException e1) {
				System.err.println("Charset name " + charsetName + " is illegal, using default!");
			} catch (UnsupportedCharsetException e2) {
				System.err.println("Charset name " + charsetName + " is not supported, using default!");
			}
		}
		
		// System.out.println("Encoding string '" + inputString + "' to charset " + charset.name());
		
		byte[] res = inputString.getBytes(charset);
		
		// System.out.println("Input len=" + inputString.length() + " to output byte array len=" + res.length);
		
		return res; 
	}
	
	public String getUTFString(byte[] inputBytes, String charsetName) {
		// change the default here if all the strings used in the project have the same encoding
		Charset charset = Charset.forName("US-ASCII");
		
		if (charsetName != null) {
			try {
				charset = Charset.forName(charsetName);
			} catch (IllegalCharsetNameException e1) {
				System.err.println("Charset name " + charsetName + " is illegal, using default!");
			} catch (UnsupportedCharsetException e2) {
				System.err.println("Charset name " + charsetName + " is not supported, using default!");
			}
		}
		
		String res = new String(inputBytes, charset);
		
		return res;
	}
	
	public void showURLInBrowser(String urlToShow)
	{
	  Intent myIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(urlToShow));
	  c.startActivity(myIntent);
	}
}
