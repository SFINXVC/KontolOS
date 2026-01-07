/*
 * KontolOS RAM Filesystem Header
 */

#ifndef RAMFS_H
#define RAMFS_H

#include "../include/types.h"

/* Filesystem constants */
#define FS_MAX_FILES        64
#define FS_MAX_FILENAME     32
#define FS_MAX_FILESIZE     (64 * 1024)  /* 64KB max per file */

/* File flags */
#define FS_FLAG_FREE        0x00
#define FS_FLAG_USED        0x01
#define FS_FLAG_READONLY    0x02
#define FS_FLAG_DIRECTORY   0x04

/* File structure */
typedef struct {
    char name[FS_MAX_FILENAME];
    uint8_t *data;
    size_t size;
    uint8_t flags;
} fs_file_t;

/* Initialize filesystem */
void fs_init(void);

/* File operations */
int fs_create(const char *name);
int fs_delete(const char *name);
fs_file_t *fs_open(const char *name);
int fs_read(fs_file_t *file, void *buffer, size_t size, size_t offset);
int fs_write(fs_file_t *file, const void *data, size_t size);
int fs_append(fs_file_t *file, const void *data, size_t size);
int fs_truncate(fs_file_t *file);

/* Directory operations */
int fs_list(char *buffer, size_t buffer_size);
int fs_exists(const char *name);
int fs_count(void);

/* Get file info */
size_t fs_get_size(fs_file_t *file);
const char *fs_get_name(fs_file_t *file);
int fs_is_dir(fs_file_t *file);

/* Directory operations */
int fs_mkdir(const char *name);
int fs_rmdir(const char *name);
int fs_chdir(const char *name);
const char *fs_getcwd(void);

#endif /* RAMFS_H */
