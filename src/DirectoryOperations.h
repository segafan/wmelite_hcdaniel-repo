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

typedef struct generic_directory_ops_t
{
	void *(*dir_open)  (const char *path);
	char *(*dir_read)  (void *handle);
	int   (*dir_close) (void *handle);
} generic_directory_ops;


#ifdef __cplusplus
}
#endif

#endif /* DIRECTORYOPERATIONS_H_ */
