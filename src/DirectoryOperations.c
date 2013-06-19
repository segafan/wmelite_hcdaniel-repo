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
#include <android/asset_manager_jni.h>
#endif

static DIRHANDLE  dir_open_plain(const char *path);
static char     *dir_read_plain(DIRHANDLE handle);
static int        dir_close_plain(DIRHANDLE handle);
static char     *dir_get_package_extension_plain(void);

#ifdef __ANDROID__

static AAssetManager *assetManager;

static DIRHANDLE  dir_open_android_asset(const char *path);
static char     *dir_read_android_asset(DIRHANDLE handle);
static int        dir_close_android_asset(DIRHANDLE handle);
static char     *dir_get_package_extension_android_asset(void);

#endif

generic_directory_ops directory_ops_plain =
{
	.dir_open                  = dir_open_plain,
	.dir_read                  = dir_read_plain,
	.dir_close                 = dir_close_plain,
	.dir_get_package_extension = dir_get_package_extension_plain
};

#ifdef __ANDROID__

generic_directory_ops directory_ops_android_asset =
{
	.dir_open                  = dir_open_android_asset,
	.dir_read                  = dir_read_android_asset,
	.dir_close                 = dir_close_android_asset,
	.dir_get_package_extension = dir_get_package_extension_android_asset
};

#endif

generic_directory_ops *get_directory_operations(dir_access_variant access_variant)
{
  if (access_variant == DIR_ACCESS_VARIANT_PLAIN)
  {
    return &directory_ops_plain;
  }
  
#ifdef __ANDROID__

  if (access_variant == DIR_ACCESS_VARIANT_ANDROID_ASSET)
  {
    return &directory_ops_android_asset;
  }
  
#endif
  
  return NULL;
}

static DIRHANDLE  dir_open_plain(const char *path)
{
	return (DIRHANDLE) opendir(path);
}

static char     *dir_read_plain(DIRHANDLE handle)
{
	struct dirent* ent = readdir((DIR*) handle);
	return ent->d_name;
}

static int        dir_close_plain(DIRHANDLE handle)
{
	return closedir((DIR*) handle);
}

static char     *dir_get_package_extension_plain(void)
{
	return "dcp";
}

#ifdef __ANDROID__

static DIRHANDLE  dir_open_android_asset(const char *path)
{
    return (DIRHANDLE) AAssetManager_openDir(assetManager, path);
}

static char     *dir_read_android_asset(DIRHANDLE handle)
{
    return AAssetDir_getNextFileName((AAssetDir *) handle);
}

static int        dir_close_android_asset(DIRHANDLE handle)
{
    AAssetDir_close((AAssetDir *) handle);
    return 0;
}

static char     *dir_get_package_extension_android_asset(void)
{
    return "dcp";    
}

#endif

