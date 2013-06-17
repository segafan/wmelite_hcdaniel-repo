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

static int        file_exists_plain(const char *name);
static FILEHANDLE file_open_plain(const char *name, const char *mode);
static long       file_read_plain(char *buffer, long size, FILEHANDLE handle);
static long       file_write_plain(const char *buffer, long size, FILEHANDLE handle);
static int        file_seek_plain(FILEHANDLE handle, long offset, int whence);
static long       file_tell_plain(FILEHANDLE handle);
static int        file_close_plain(FILEHANDLE handle);


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

generic_file_ops *get_file_operations(file_access_variant access_variant)
{
	return &file_ops_plain;
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
