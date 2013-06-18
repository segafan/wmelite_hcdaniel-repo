/*
 * FileOperations.c
 *
 *  Created on: 16.06.2013
 *      Author: daniel
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "FileOperations.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

static int        file_exists_plain(const char *name);
static FILEHANDLE file_open_plain(const char *name, const char *mode);
static long       file_read_plain(char *buffer, long size, FILEHANDLE handle);
static long       file_write_plain(const char *buffer, long size, FILEHANDLE handle);
static int        file_seek_plain(FILEHANDLE handle, long offset, int whence);
static long       file_tell_plain(FILEHANDLE handle);
static int        file_close_plain(FILEHANDLE handle);

#ifdef __ANDROID__

static AAssetManager *assetManager;

static int        file_exists_android_asset(const char *name);
static FILEHANDLE file_open_android_asset(const char *name, const char *mode);
static long       file_read_android_asset(char *buffer, long size, FILEHANDLE handle);
static long       file_write_android_asset(const char *buffer, long size, FILEHANDLE handle);
static int        file_seek_android_asset(FILEHANDLE handle, long offset, int whence);
static long       file_tell_android_asset(FILEHANDLE handle);
static int        file_close_android_asset(FILEHANDLE handle);

#endif


generic_file_ops file_ops_plain =
{
	.file_exists = file_exists_plain,
	.file_open   = file_open_plain,
	.file_read   = file_read_plain,
	.file_write  = file_write_plain,
	.file_seek   = file_seek_plain,
	.file_tell   = file_tell_plain,
	.file_close  = file_close_plain
};

#ifdef __ANDROID__

generic_file_ops file_ops_android_asset =
{
	.file_exists = file_exists_android_asset,
	.file_open   = file_open_android_asset,
	.file_read   = file_read_android_asset,
	.file_write  = file_write_android_asset,
	.file_seek   = file_seek_android_asset,
	.file_tell   = file_tell_android_asset,
	.file_close  = file_close_android_asset
};

#endif

generic_file_ops *get_file_operations(file_access_variant access_variant)
{
  if (access_variant == FILE_ACCESS_VARIANT_PLAIN)
  {
    return &file_ops_plain;
  }
  
#ifdef __ANDROID__

  if (access_variant == FILE_ACCESS_VARIANT_ANDROID_ASSET)
  {
    return &file_ops_plain;
  }
  
#endif

  return NULL;
}

static int        file_exists_plain(const char *name)
{
	return access(name, R_OK);
}

static FILEHANDLE file_open_plain(const char *name, const char *mode)
{
	return (FILEHANDLE) fopen(name, mode);
}

static long       file_read_plain(char *buffer, long size, FILEHANDLE handle)
{
	return fread(buffer, size, 1, (FILE*) handle);
}

static long       file_write_plain(const char *buffer, long size, FILEHANDLE handle)
{
	return fwrite(buffer, size, 1, (FILE*) handle);
}

static int        file_seek_plain(FILEHANDLE handle, long offset, int whence)
{
	return fseek((FILE*) handle, offset, whence);
}

static long       file_tell_plain(FILEHANDLE handle)
{
	return ftell((FILE*) handle);
}

static int        file_close_plain(FILEHANDLE handle)
{
	return fclose((FILE*) handle);
}

#ifdef __ANDROID__

static int        file_exists_android_asset(const char *name)
{
    int retval = 0;
    AAsset *asset = AAssetManager_open(assetManager, name, AASSET_MODE_BUFFER);
    if (asset == NULL)
    {
        retval = -1;
    }
    else
    {
        AAsset_close(asset);   
    }
    
    return retval;
}

static FILEHANDLE file_open_android_asset(const char *name, const char *mode)
{
    // ignore the supplied mode flags
    return (FILEHANDLE) AAssetManager_open(assetManager, name, AASSET_MODE_BUFFER);
}

static long       file_read_android_asset(char *buffer, long size, FILEHANDLE handle)
{
    return AAsset_read((AAsset *) handle, buffer, size);
}

static long       file_write_android_asset(const char *buffer, long size, FILEHANDLE handle)
{
    // writing is not possible
    return 0;
}

static int        file_seek_android_asset(FILEHANDLE handle, long offset, int whence)
{
    return AAsset_seek((AAsset *) handle, offset, whence);
}

static long       file_tell_android_asset(FILEHANDLE handle)
{
    return (AAsset_getLength((AAsset *) handle) - AAsset_getRemainingLength((AAsset *) handle));
}

static int        file_close_android_asset(FILEHANDLE handle)
{
    AAsset_close((AAsset *) handle);
    return 0;
}

#endif


