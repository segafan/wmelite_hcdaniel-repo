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

typedef struct generic_file_ops_t
{
	int   (*file_exists) (const char *name);
	void *(*file_open)   (const char *name, const char *mode);
	long  (*file_read)   (char *buffer, long size, void *handle);
	long  (*file_write)  (const char *buffer, long size, void *handle);
	int   (*file_seek)   (void *handle, long offset, int whence);
	long  (*file_tell)   (void *handle);
	int   (*file_close)  (void *handle);
} generic_file_ops;

#ifdef __cplusplus
}
#endif

#endif /* FILEOPERATIONS_H_ */
