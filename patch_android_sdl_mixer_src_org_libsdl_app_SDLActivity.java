--- android_sdl_mixer/src/org/libsdl/app/SDLActivity.java.orig	2015-05-30 19:40:43.384818361 +0200
+++ android_sdl_mixer/src/org/libsdl/app/SDLActivity.java	2015-05-30 19:37:35.788809018 +0200
@@ -28,6 +28,9 @@
 import android.media.*;
 import android.hardware.*;
 
+// WMELite addition
+import org.deadcode.wmelite.WMELiteFunctions;
+
 /**
     SDL Activity
 */
@@ -58,6 +61,9 @@
     // Audio
     protected static AudioTrack mAudioTrack;
 
+    // WMELite added
+    protected static final WMELiteFunctions wmeLiteFuncs = new WMELiteFunctions();
+
     /**
      * This method is called by SDL before loading the native shared libraries.
      * It can be overridden to provide names of shared libraries to be loaded.
@@ -68,11 +74,15 @@
      */
     protected String[] getLibraries() {
         return new String[] {
+            "gnustl_shared",    // WMELite additional libraries added
+            "freeimage",
             "SDL2",
             // "SDL2_image",
-            // "SDL2_mixer",
+            "SDL2_mixer",
             // "SDL2_net",
             // "SDL2_ttf",
+            "effectsprivate",
+            "echo",
             "main"
         };
     }
@@ -119,6 +129,9 @@
         Log.v(TAG, "onCreate():" + mSingleton);
         super.onCreate(savedInstanceState);
 
+        // WMELite added
+        wmeLiteFuncs.setContext(getApplicationContext());
+        
         SDLActivity.initialize();
         // So we can call stuff from static callbacks
         mSingleton = this;
@@ -162,6 +175,13 @@
         // Set up the surface
         mSurface = new SDLSurface(getApplication());
 
+        nativeAddHintCallback(SDL_HINT_ANDROID_USE_UI_LOW_PROFILE, new SDLHintCallback() {
+        	@Override
+        	public void callback(String name, String oldValue, String newValue) {
+        		updateLowProfileSettings(newValue);
+        	}
+        });
+        
         if(Build.VERSION.SDK_INT >= 12) {
             mJoystickHandler = new SDLJoystickHandler_API12();
         }
@@ -209,6 +229,8 @@
         }
 
         SDLActivity.handleResume();
+        
+        updateLowProfileSettings(nativeGetHint(SDL_HINT_ANDROID_USE_UI_LOW_PROFILE));
     }
 
 
@@ -321,6 +343,20 @@
         mSingleton.finish();
     }
 
+    private static final String SDL_HINT_ANDROID_USE_UI_LOW_PROFILE = "SDL_ANDROID_USE_UI_LOW_PROFILE";
+    		
+    void updateLowProfileSettings(String value) {
+    	if ("1".equals(value)) {
+    		runOnUiThread(new Runnable() {
+    			@Override
+    			public void run() {
+    				if (Build.VERSION.SDK_INT >= 14) {
+    					getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
+    				}
+    			}
+    		});
+    	}
+    }
 
     // Messages from the SDLMain thread
     static final int COMMAND_CHANGE_TITLE = 1;
@@ -431,6 +467,11 @@
                                                int is_accelerometer, int nbuttons,
                                                int naxes, int nhats, int nballs);
     public static native int nativeRemoveJoystick(int device_id);
+
+    interface SDLHintCallback {
+    	void callback(String name, String oldValue, String newValue);
+    }
+    public static native void nativeAddHintCallback(String name, SDLHintCallback callback);
     public static native String nativeGetHint(String name);
 
     /**
@@ -929,6 +970,9 @@
 class SDLMain implements Runnable {
     @Override
     public void run() {
+    	// WMELite added
+    	SDLActivity.wmeLiteFuncs.init();
+
         // Runs SDL_main()
         SDLActivity.nativeInit(SDLActivity.mSingleton.getArguments());
 
