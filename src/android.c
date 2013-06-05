
#include "android.h"

#include "SDL_android.h"

#include <android/log.h>

const char* className_wmeliteFunctions = "org/deadcode/wmelite";

static jobject callbackObject;

static JNIEnv *localEnv;

void Java_org_deadcode_wmelite_WMELiteFunctions_nativeInit(JNIEnv* env, jobject o)
{

	callbackObject = (*env)->NewGlobalRef(env, o);

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "Global ref to WMELITE=%s", (callbackObject == NULL) ? "NULL" : "OK");

	// localEnv = env;
}

void android_setLocalEnv(JNIEnv *env)
{
	localEnv = env;
}

void android_getLogFileDirectory(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getLogFileDirectory", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getLogFileDirectory() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getPrivateFilesPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getPrivateFilesPath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getPrivateFilesPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getDeviceTypeHint(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getDeviceTypeHint", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getDeviceTypeHint() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getGamePackagePath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() env=%s", (env == NULL) ? "NULL" : "OK");

	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getGamePackagePath", "()Ljava/lang/String;");

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() callbackid=%d", callbackID);

	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() jstring=%s", (str == NULL) ? "NULL" : "OK");

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getGameFilePath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getGameFilePath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGameFilePath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getFontPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = localEnv;
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getFontPath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getFontPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}
