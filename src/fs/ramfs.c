/*
 * KontolOS RAM Filesystem Implementation
 */

#include "ramfs.h"
#include "../kernel/memory.h"
#include "../lib/string.h"

/* File table */
static fs_file_t file_table[FS_MAX_FILES];
static int fs_initialized = 0;

/*
 * Initialize the filesystem
 */
void fs_init(void)
{
    for (int i = 0; i < FS_MAX_FILES; i++) {
        file_table[i].name[0] = '\0';
        file_table[i].data = NULL;
        file_table[i].size = 0;
        file_table[i].flags = FS_FLAG_FREE;
    }
    fs_initialized = 1;
}

/*
 * Find a file by name, returns index or -1 if not found
 */
static int fs_find(const char *name)
{
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if ((file_table[i].flags & FS_FLAG_USED) && 
            strcmp(file_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * Find a free slot in file table
 */
static int fs_find_free(void)
{
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!(file_table[i].flags & FS_FLAG_USED)) {
            return i;
        }
    }
    return -1;
}

/*
 * Create a new file
 */
int fs_create(const char *name)
{
    if (!name || strlen(name) == 0 || strlen(name) >= FS_MAX_FILENAME) {
        return -1;  /* Invalid name */
    }

    /* Check if file already exists */
    if (fs_find(name) >= 0) {
        return -2;  /* File already exists */
    }

    /* Find free slot */
    int slot = fs_find_free();
    if (slot < 0) {
        return -3;  /* No free slots */
    }

    /* Initialize file */
    strncpy(file_table[slot].name, name, FS_MAX_FILENAME - 1);
    file_table[slot].name[FS_MAX_FILENAME - 1] = '\0';
    file_table[slot].data = NULL;
    file_table[slot].size = 0;
    file_table[slot].flags = FS_FLAG_USED;

    return 0;
}

/*
 * Delete a file
 */
int fs_delete(const char *name)
{
    int idx = fs_find(name);
    if (idx < 0) {
        return -1;  /* File not found */
    }

    /* Free file data */
    if (file_table[idx].data != NULL) {
        kfree(file_table[idx].data);
    }

    /* Clear file entry */
    file_table[idx].name[0] = '\0';
    file_table[idx].data = NULL;
    file_table[idx].size = 0;
    file_table[idx].flags = FS_FLAG_FREE;

    return 0;
}

/*
 * Open a file (returns pointer to file structure)
 */
fs_file_t *fs_open(const char *name)
{
    int idx = fs_find(name);
    if (idx < 0) {
        return NULL;
    }
    return &file_table[idx];
}

/*
 * Read from a file
 */
int fs_read(fs_file_t *file, void *buffer, size_t size, size_t offset)
{
    if (!file || !buffer) {
        return -1;
    }

    if (offset >= file->size) {
        return 0;  /* Nothing to read */
    }

    size_t available = file->size - offset;
    size_t to_read = (size < available) ? size : available;

    if (file->data != NULL) {
        memcpy(buffer, file->data + offset, to_read);
    }

    return (int)to_read;
}

/*
 * Write to a file (overwrites existing content)
 */
int fs_write(fs_file_t *file, const void *data, size_t size)
{
    if (!file || !data) {
        return -1;
    }

    if (size > FS_MAX_FILESIZE) {
        return -2;  /* Too large */
    }

    /* Free old data */
    if (file->data != NULL) {
        kfree(file->data);
        file->data = NULL;
    }

    /* Allocate new buffer */
    if (size > 0) {
        file->data = kmalloc(size);
        if (file->data == NULL) {
            return -3;  /* Out of memory */
        }
        memcpy(file->data, data, size);
    }

    file->size = size;
    return (int)size;
}

/*
 * Append data to a file
 */
int fs_append(fs_file_t *file, const void *data, size_t size)
{
    if (!file || !data || size == 0) {
        return -1;
    }

    size_t new_size = file->size + size;
    if (new_size > FS_MAX_FILESIZE) {
        return -2;  /* Would be too large */
    }

    /* Allocate new buffer */
    uint8_t *new_data = kmalloc(new_size);
    if (new_data == NULL) {
        return -3;  /* Out of memory */
    }

    /* Copy old data */
    if (file->data != NULL && file->size > 0) {
        memcpy(new_data, file->data, file->size);
    }

    /* Append new data */
    memcpy(new_data + file->size, data, size);

    /* Free old data */
    if (file->data != NULL) {
        kfree(file->data);
    }

    file->data = new_data;
    file->size = new_size;

    return (int)size;
}

/*
 * Truncate a file (clear contents)
 */
int fs_truncate(fs_file_t *file)
{
    if (!file) {
        return -1;
    }

    if (file->data != NULL) {
        kfree(file->data);
        file->data = NULL;
    }
    file->size = 0;

    return 0;
}

/*
 * List all files (writes to buffer)
 */
int fs_list(char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) {
        return -1;
    }

    buffer[0] = '\0';
    size_t pos = 0;
    int count = 0;

    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].flags & FS_FLAG_USED) {
            size_t name_len = strlen(file_table[i].name);
            if (pos + name_len + 2 < buffer_size) {
                strcpy(buffer + pos, file_table[i].name);
                pos += name_len;
                buffer[pos++] = '\n';
                buffer[pos] = '\0';
                count++;
            }
        }
    }

    return count;
}

/*
 * Check if a file exists
 */
int fs_exists(const char *name)
{
    return fs_find(name) >= 0;
}

/*
 * Get number of files
 */
int fs_count(void)
{
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].flags & FS_FLAG_USED) {
            count++;
        }
    }
    return count;
}

/*
 * Get file size
 */
size_t fs_get_size(fs_file_t *file)
{
    return file ? file->size : 0;
}

/*
 * Get file name
 */
const char *fs_get_name(fs_file_t *file)
{
    return file ? file->name : NULL;
}

/*
 * Check if file is a directory
 */
int fs_is_dir(fs_file_t *file)
{
    return file ? (file->flags & FS_FLAG_DIRECTORY) != 0 : 0;
}

/* Current working directory */
static char current_dir[FS_MAX_FILENAME] = "/";

/*
 * Create a directory
 */
int fs_mkdir(const char *name)
{
    if (!name || strlen(name) == 0 || strlen(name) >= FS_MAX_FILENAME) {
        return -1;  /* Invalid name */
    }

    /* Check if already exists */
    if (fs_find(name) >= 0) {
        return -2;  /* Already exists */
    }

    /* Find free slot */
    int slot = fs_find_free();
    if (slot < 0) {
        return -3;  /* No free slots */
    }

    /* Initialize directory entry */
    strncpy(file_table[slot].name, name, FS_MAX_FILENAME - 1);
    file_table[slot].name[FS_MAX_FILENAME - 1] = '\0';
    file_table[slot].data = NULL;
    file_table[slot].size = 0;
    file_table[slot].flags = FS_FLAG_USED | FS_FLAG_DIRECTORY;

    return 0;
}

/*
 * Remove a directory
 */
int fs_rmdir(const char *name)
{
    int idx = fs_find(name);
    if (idx < 0) {
        return -1;  /* Not found */
    }

    /* Check if it's a directory */
    if (!(file_table[idx].flags & FS_FLAG_DIRECTORY)) {
        return -2;  /* Not a directory */
    }

    /* Clear directory entry */
    file_table[idx].name[0] = '\0';
    file_table[idx].data = NULL;
    file_table[idx].size = 0;
    file_table[idx].flags = FS_FLAG_FREE;

    return 0;
}

/*
 * Change current directory
 */
int fs_chdir(const char *name)
{
    if (!name) {
        return -1;
    }

    /* Handle root directory */
    if (strcmp(name, "/") == 0) {
        strcpy(current_dir, "/");
        return 0;
    }

    /* Handle ".." (parent directory) */
    if (strcmp(name, "..") == 0) {
        strcpy(current_dir, "/");
        return 0;
    }

    /* Check if directory exists */
    int idx = fs_find(name);
    if (idx < 0) {
        return -1;  /* Not found */
    }

    if (!(file_table[idx].flags & FS_FLAG_DIRECTORY)) {
        return -2;  /* Not a directory */
    }

    strncpy(current_dir, name, FS_MAX_FILENAME - 1);
    current_dir[FS_MAX_FILENAME - 1] = '\0';

    return 0;
}

/*
 * Get current working directory
 */
const char *fs_getcwd(void)
{
    return current_dir;
}
