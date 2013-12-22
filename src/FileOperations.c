/*
 * FileOperations.c
 *
 *  Created on: 16.06.2013
 *      Author: daniel
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "SDL.h"

#ifdef __WIN32__
#	include <direct.h>
#	include <io.h>
#else
#	include <unistd.h>
#endif

#include "FileOperations.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/log.h>
#endif

static int        file_exists_plain(const char *name);
static FILEHANDLE file_open_plain(const char *name, const char *mode);
static long       file_read_plain(char *buffer, long size, FILEHANDLE handle);
static long       file_write_plain(const char *buffer, long size, FILEHANDLE handle);
static long       file_print_plain(FILEHANDLE handle, const char *format, ...);
static int        file_putc_plain(int character, FILEHANDLE handle);
static int        file_seek_plain(FILEHANDLE handle, long offset, int whence);
static long       file_tell_plain(FILEHANDLE handle);
static int        file_error_plain(FILEHANDLE handle);
static int        file_close_plain(FILEHANDLE handle);

#ifdef __ANDROID__

extern AAssetManager *assetManager;

static int        file_exists_android_asset(const char *name);
static FILEHANDLE file_open_android_asset(const char *name, const char *mode);
static long       file_read_android_asset(char *buffer, long size, FILEHANDLE handle);
static long       file_write_android_asset(const char *buffer, long size, FILEHANDLE handle);
static long       file_print_android_asset(FILEHANDLE handle, const char *format, ...);
static int        file_putc_android_asset(int character, FILEHANDLE handle);
static int        file_seek_android_asset(FILEHANDLE handle, long offset, int whence);
static long       file_tell_android_asset(FILEHANDLE handle);
static int        file_error_android_asset(FILEHANDLE handle);
static int        file_close_android_asset(FILEHANDLE handle);

static int        file_exists_android_obb_plain(const char *name);
static FILEHANDLE file_open_android_obb_plain(const char *name, const char *mode);
static long       file_read_android_obb_plain(char *buffer, long size, FILEHANDLE handle);
static long       file_write_android_obb_plain(const char *buffer, long size, FILEHANDLE handle);
static long       file_print_android_obb_plain(FILEHANDLE handle, const char *format, ...);
static int        file_putc_android_obb_plain(int character, FILEHANDLE handle);
static int        file_seek_android_obb_plain(FILEHANDLE handle, long offset, int whence);
static long       file_tell_android_obb_plain(FILEHANDLE handle);
static int        file_error_android_obb_plain(FILEHANDLE handle);
static int        file_close_android_obb_plain(FILEHANDLE handle);

#endif


generic_file_ops file_ops_plain =
{
	file_exists_plain,
	file_open_plain,
	file_read_plain,
	file_write_plain,
	file_print_plain,
	file_putc_plain,
	file_seek_plain,
	file_tell_plain,
	file_error_plain,
	file_close_plain
};

#ifdef __ANDROID__

generic_file_ops file_ops_android_asset =
{
  .file_exists = file_exists_android_asset,
  .file_open   = file_open_android_asset,
  .file_read   = file_read_android_asset,
  .file_write  = file_write_android_asset,
  .file_print  = file_print_android_asset,
  .file_putc   = file_putc_android_asset,
  .file_seek   = file_seek_android_asset,
  .file_tell   = file_tell_android_asset,
  .file_error  = file_error_android_asset,
  .file_close  = file_close_android_asset
};

generic_file_ops file_ops_android_obb_plain =
{
  .file_exists = file_exists_android_obb_plain,
  .file_open   = file_open_android_obb_plain,
  .file_read   = file_read_android_obb_plain,
  .file_write  = file_write_android_obb_plain,
  .file_print  = file_print_android_obb_plain,
  .file_putc   = file_putc_android_obb_plain,
  .file_seek   = file_seek_android_obb_plain,
  .file_tell   = file_tell_android_obb_plain,
  .file_error  = file_error_android_obb_plain,
  .file_close  = file_close_android_obb_plain
};

#endif

generic_file_ops *get_file_operations(file_access_variant access_variant)
{
  if (access_variant == FILE_ACCESS_VARIANT_PLAIN)
  {
#ifdef __ANDROID__
	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "FileOperations: Requested PLAIN access.");
#endif
    return &file_ops_plain;
  }

#ifdef __ANDROID__

  if (access_variant == FILE_ACCESS_VARIANT_ANDROID_ASSET)
  {
    // __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "FileOperations: Requested ANDROID ASSET access.");
    return &file_ops_android_asset;
  }
  if (access_variant == FILE_ACCESS_VARIANT_ANDROID_OBB_PLAIN)
  {
    // __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "FileOperations: Requested OBB PLAIN access.");
    return &file_ops_android_obb_plain;
  }

#endif

  return NULL;
}

static int        file_exists_plain(const char *name)
{
#ifdef __WIN32__
	return _access(name, 0);
#else
	return access(name, R_OK);
#endif
}

static FILEHANDLE file_open_plain(const char *name, const char *mode)
{
	return (FILEHANDLE) fopen(name, mode);
}

static long       file_read_plain(char *buffer, long size, FILEHANDLE handle)
{
	return fread(buffer, 1, size, (FILE*) handle);
}

static long       file_write_plain(const char *buffer, long size, FILEHANDLE handle)
{
	return fwrite(buffer, 1, size, (FILE*) handle);
}

static long       file_print_plain(FILEHANDLE handle, const char *format, ...)
{
	int retval;
	va_list arglist;
	va_start( arglist, format );
	retval = vfprintf((FILE*) handle, format, arglist);
	va_end(arglist);

	return retval;
}

static int        file_putc_plain(int character, FILEHANDLE handle)
{
	return fputc(character, (FILE*) handle);
}

static int        file_seek_plain(FILEHANDLE handle, long offset, int whence)
{
	return fseek((FILE*) handle, offset, whence);
}

static long       file_tell_plain(FILEHANDLE handle)
{
	return ftell((FILE*) handle);
}

static int        file_error_plain(FILEHANDLE handle)
{
	return ferror((FILE*) handle);
}

static int        file_close_plain(FILEHANDLE handle)
{
	return fclose((FILE*) handle);
}

#ifdef __ANDROID__

char buffer[32768];

static int        file_exists_android_asset(const char *name)
{
	int i;
	int retval = 0;

    // skip the "asset://" prefix and remove a possible trailing slash
	int len = strlen(name);
	strcpy(buffer, name + 8);
	len = len - 8;
	if ((buffer[len - 1] == '/') || (buffer[len - 1] == '\\'))
	{
		buffer[len - 1] = 0;
	}
	for (i = 0; i < len; i++)
	{
		if (buffer[i] == '\\')
		{
			buffer[i] = '/';
		}
	}

	AAsset *asset = AAssetManager_open(assetManager, buffer, AASSET_MODE_BUFFER);
    if (asset == NULL)
    {
    	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: Request to open asset at path: %s FAILED", buffer);
       retval = -1;
    }
    else
    {
    	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: Request to open asset at path: %s OK", buffer);
        AAsset_close(asset);
    }

    return retval;
}

static FILEHANDLE file_open_android_asset(const char *name, const char *mode)
{
    // ignore the supplied mode flags

    // skip the "asset://" prefix and remove a possible trailing slash
	int i;
	int len = strlen(name);
	strcpy(buffer, name + 8);
	len = len - 8;
	if ((buffer[len - 1] == '/') || (buffer[len - 1] == '\\'))
	{
		buffer[len - 1] = 0;
	}
	for (i = 0; i < len; i++)
	{
		if (buffer[i] == '\\')
		{
			buffer[i] = '/';
		}
	}

	FILEHANDLE handle = (FILEHANDLE) AAssetManager_open(assetManager, buffer, AASSET_MODE_BUFFER);
	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: Request to open asset at path: %s success=%s", buffer, (handle == NULL) ? "FALSE" : "TRUE");
    return handle;
}

static long       file_read_android_asset(char *buffer, long size, FILEHANDLE handle)
{
	long retval = AAsset_read((AAsset *) handle, buffer, size);
	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: read=%ld", retval);
    return retval;
}

static long       file_write_android_asset(const char *buffer, long size, FILEHANDLE handle)
{
    // writing is not possible
    return 0;
}

static long       file_print_android_asset(FILEHANDLE handle, const char *format, ...)
{
    // writing is not possible
    return 0;
}

static int        file_putc_android_asset(int character, FILEHANDLE handle)
{
    // writing is not possible
    return 0;
}

static int        file_seek_android_asset(FILEHANDLE handle, long offset, int whence)
{
    int retval = AAsset_seek((AAsset *) handle, offset, whence);
	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: seek=%d", retval);

	// need to align to original fseek() return value
	if (retval >= 0)
	{
		retval = 0;
	}

	return retval;
}

static long       file_tell_android_asset(FILEHANDLE handle)
{
	off_t length;
	off_t remainingLength;
	long retval;

	length = AAsset_getLength((AAsset *) handle);
	remainingLength = AAsset_getRemainingLength((AAsset *) handle);

	retval = length - remainingLength;

	/*
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "AssetFile: tell len=%d remainingLen=%d tell=%ld",
			length, remainingLength, retval);
	*/

	return retval;
}

static int        file_error_android_asset(FILEHANDLE handle)
{
	return 0;
}

static int        file_close_android_asset(FILEHANDLE handle)
{
    AAsset_close((AAsset *) handle);
    return 0;
}

static int        file_exists_android_obb_plain(const char *name)
{
  // skip the "obbplain://" prefix and remove a possible trailing slash
  int i;
  int len = strlen(name);
  strcpy(buffer, name + 11);
  len = len - 11;
  if ((buffer[len - 1] == '/') || (buffer[len - 1] == '\\'))
  {
    buffer[len - 1] = 0;
  }
  for (i = 0; i < len; i++)
  {
    if (buffer[i] == '\\')
    {
      buffer[i] = '/';
    }
  }

  return access(buffer, R_OK);
}

static FILEHANDLE file_open_android_obb_plain(const char *name, const char *mode)
{
  // skip the "obbplain://" prefix and remove a possible trailing slash
  int i;
  int len = strlen(name);
  strcpy(buffer, name + 11);
  len = len - 11;
  if ((buffer[len - 1] == '/') || (buffer[len - 1] == '\\'))
  {
    buffer[len - 1] = 0;
  }
  for (i = 0; i < len; i++)
  {
    if (buffer[i] == '\\')
    {
      buffer[i] = '/';
    }
  }

  // do not open the file for writing or appending, only reading is good
  if ((mode[0] != 'r') && (mode[0] != 'R'))
  {
    return NULL;
  }

  return (FILEHANDLE) fopen(buffer, mode);
}

static long       file_read_android_obb_plain(char *buffer, long size, FILEHANDLE handle)
{
  return fread(buffer, 1, size, (FILE*) handle);
}

static long       file_write_android_obb_plain(const char *buffer, long size, FILEHANDLE handle)
{
  // OBB files shall never be written to
  return 0;
}

static long       file_print_android_obb_plain(FILEHANDLE handle, const char *format, ...)
{
  // OBB files shall never be written to
  return 0;
}

static int        file_putc_android_obb_plain(int character, FILEHANDLE handle)
{
  // OBB files shall never be written to
  return 0;
}

static int        file_seek_android_obb_plain(FILEHANDLE handle, long offset, int whence)
{
  return fseek((FILE*) handle, offset, whence);
}

static long       file_tell_android_obb_plain(FILEHANDLE handle)
{
  return ftell((FILE*) handle);
}

static int        file_error_android_obb_plain(FILEHANDLE handle)
{
  return ferror((FILE*) handle);
}

static int        file_close_android_obb_plain(FILEHANDLE handle)
{
  return fclose((FILE*) handle);
}

#endif
