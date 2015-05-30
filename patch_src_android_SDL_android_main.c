--- src/android/SDL_android_main.c	2015-03-28 11:40:56.255750027 +0100
+++ src/android/SDL_android_main.c.orig	2015-05-30 19:20:42.952758575 +0200
@@ -1,9 +1,7 @@
 /*
     SDL_android_main.c, placed in the public domain by Sam Lantinga  3/13/14
 */
-
-// WMELite path changed
-#include "SDL_internal.h"
+#include "../../SDL_internal.h"
 
 #ifdef __ANDROID__
 
@@ -19,8 +17,7 @@
 extern void SDL_Android_Init(JNIEnv* env, jclass cls);
 
 /* Start up the SDL app */
-// WMELite add JNI definitions
-JNIEXPORT int JNICALL Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject array)
+int Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject array)
 {
     int i;
     int argc;
