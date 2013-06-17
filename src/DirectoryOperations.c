/*
 * DirectoryOperations.c
 *
 *  Created on: 16.06.2013
 *      Author: daniel
 */

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include "DirectoryOperations.h"

static DIRHANDLE  dir_open_plain(const char *path);
static char     *dir_read_plain(DIRHANDLE handle);
static int        dir_close_plain(DIRHANDLE handle);
static char     *dir_get_package_extension_plain(void);

generic_directory_ops directory_ops_plain =
{
	.dir_open                  = dir_open_plain,
	.dir_read                  = dir_read_plain,
	.dir_close                 = dir_close_plain,
	.dir_get_package_extension = dir_get_package_extension_plain
};

generic_directory_ops *get_directory_operations(dir_access_variant access_variant)
{
	return &directory_ops_plain;
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
