/*
 * FileOperations.h
 *
 *  Created on: 15.06.2013
 *      Author: daniel
 */

#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_

/* This is for C++ and does no harm in C */
#ifdef __cplusplus
extern "C" {
#endif

typedef void* FILEHANDLE;

typedef enum file_access_variant_t
{
	FILE_ACCESS_VARIANT_PLAIN             = 0,
	FILE_ACCESS_VARIANT_ANDROID_ASSET     = 1,
	FILE_ACCESS_VARIANT_ANDROID_OBB_PLAIN = 2,
	FILE_ACCESS_VARIANT_ANDROID_OBB_MOUNT = 3
} file_access_variant;

typedef struct generic_file_ops_t
{
	int        (*file_exists) (const char *name);
	FILEHANDLE (*file_open)   (const char *name, const char *mode);
	long       (*file_read)   (char *buffer, long size, FILEHANDLE handle);
	long       (*file_write)  (const char *buffer, long size, FILEHANDLE handle);
	int        (*file_seek)   (FILEHANDLE handle, long offset, int whence);
	long       (*file_tell)   (FILEHANDLE handle);
	int        (*file_close)  (FILEHANDLE handle);
} generic_file_ops;

generic_file_ops get_file_operations(file_access_variant access_variant);

#ifdef __cplusplus
}
#endif

#endif /* FILEOPERATIONS_H_ */
