/*
 * DirectoryOperations.c
 *
 *  Created on: 16.06.2013
 *      Author: daniel
 */

#include "SDL.h"

#ifdef __WIN32__
#	include <shlobj.h>
#	include <io.h>
#else
#	include <stdlib.h>
#	include <sys/types.h>
#	include <dirent.h>
#endif

#include "DirectoryOperations.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/log.h>
#endif

static DIRHANDLE  dir_open_plain(const char *path);
static char     *dir_find_plain(DIRHANDLE handle);
static int        dir_close_plain(DIRHANDLE handle);
static char     *dir_get_package_extension_plain(void);

#ifdef __ANDROID__

extern AAssetManager *assetManager;

static DIRHANDLE  dir_open_android_asset(const char *path);
static char     *dir_find_android_asset(DIRHANDLE handle);
static int        dir_close_android_asset(DIRHANDLE handle);
static char     *dir_get_package_extension_android_asset(void);

#endif

generic_directory_ops directory_ops_plain =
{
	dir_open_plain,
	dir_find_plain,
	dir_close_plain,
	dir_get_package_extension_plain
};

#ifdef __ANDROID__

generic_directory_ops directory_ops_android_asset =
{
	.dir_open                  = dir_open_android_asset,
	.dir_find                  = dir_find_android_asset,
	.dir_close                 = dir_close_android_asset,
	.dir_get_package_extension = dir_get_package_extension_android_asset
};

#endif

generic_directory_ops *get_directory_operations(dir_access_variant access_variant)
{
  if (access_variant == DIR_ACCESS_VARIANT_PLAIN)
  {
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "DirectoryOperations: Requested PLAIN access.");
#endif
	return &directory_ops_plain;
  }
  
#ifdef __ANDROID__

  if (access_variant == DIR_ACCESS_VARIANT_ANDROID_ASSET)
  {
		__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "DirectoryOperations: Requested ANDROID ASSET access.");
    return &directory_ops_android_asset;
  }
  
#endif
  
  return NULL;
}

static DIRHANDLE  dir_open_plain(const char *path)
{
#ifndef _WIN32
	DIRHANDLE handle = (DIRHANDLE) opendir(path);
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "DirectoryOperations: opendir(%s) success=%s", path, (handle == NULL) ? "FALSE" : "TRUE");
#endif
	return handle;
#else
	return NULL;
#endif
}

static char     *dir_find_plain(DIRHANDLE handle)
{
#ifndef _WIN32
	struct dirent* ent = readdir((DIR*) handle);
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "DirectoryOperations: readdir returns=%s", (ent == NULL) ? "NULL" : ent->d_name);
#endif
	if (ent != NULL) {
		return ent->d_name;
	}
	return NULL;
#else
	return NULL;
#endif
}

static int        dir_close_plain(DIRHANDLE handle)
{
#ifndef _WIN32
	return closedir((DIR*) handle);
#else
	return -1;
#endif
}

static char     *dir_get_package_extension_plain(void)
{
	return "dcp";
}

#ifdef __ANDROID__

char buffer[32768];

static DIRHANDLE  dir_open_android_asset(const char *path)
{
	// skip the "asset://" prefix and remove a possible trailing slash
	int len = strlen(path);
	strcpy(buffer, path + 8);
	len = len - 8;
	if ((buffer[len - 1] == '/') || (buffer[len - 1] == '\\'))
	{
		buffer[len - 1] = 0;
	}
	DIRHANDLE handle = (DIRHANDLE) AAssetManager_openDir(assetManager, buffer);
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetDir: Request to open asset at path: %s success=%s", buffer, (handle == NULL) ? "FALSE" : "TRUE");
    return handle;
}

static char     *dir_find_android_asset(DIRHANDLE handle)
{
	char *next = AAssetDir_getNextFileName((AAssetDir *) handle);
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetDir: next filename=%s", (next == NULL) ? "NULL" : next);
    return next;
}

static int        dir_close_android_asset(DIRHANDLE handle)
{
    AAssetDir_close((AAssetDir *) handle);
    __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetDir: close handle");
    return 0;
}

static char     *dir_get_package_extension_android_asset(void)
{
	// choose an extension that will not be compressed by android build tools
    return "png";
}

#endif

