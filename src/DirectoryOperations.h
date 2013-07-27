/*
 * DirectoryOperations.h
 *
 *  Created on: 15.06.2013
 *      Author: daniel
 */

#ifndef DIRECTORYOPERATIONS_H_
#define DIRECTORYOPERATIONS_H_

/* This is for C++ and does no harm in C */
#ifdef __cplusplus
extern "C" {
#endif

typedef void* DIRHANDLE;

typedef enum dir_access_variant_t
{
	DIR_ACCESS_VARIANT_PLAIN             = 0,
	DIR_ACCESS_VARIANT_ANDROID_ASSET     = 1
} dir_access_variant;

typedef struct generic_directory_ops_t
{
	DIRHANDLE (*dir_open)  (const char *path);
	char     *(*dir_find)  (DIRHANDLE handle);
	int       (*dir_close) (DIRHANDLE handle);
	char     *(*dir_get_package_extension) (void);
} generic_directory_ops;

generic_directory_ops *get_directory_operations(dir_access_variant access_variant);

#ifdef __cplusplus
}
#endif

#endif /* DIRECTORYOPERATIONS_H_ */
