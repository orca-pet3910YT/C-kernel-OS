#include <stdint.h>
#ifndef VFS_H
#define VFS_H
char *resolve_path(char *path, char *cwd);
typedef struct mount {
	char path[256];
	struct filesystem *fs;
} mount_t;
typedef struct file {
	uint32_t offset; // reserved for seeking
	void *data;
	uint32_t size;
	char name[256];
} file_t;
typedef struct filesystem {
	file_t(*read)(file_t*);
	int(*write)(file_t*, void*, int);
	int(*mount)(struct filesystem*);
	int(*unmount)(struct filesystem*);
	int(*list_dir)(char *directory, char **output);
	struct mount *mountpoint;
} filesystem_t;
int register_fs(filesystem_t *fs);
int unregister_fs(filesystem_t *fs);
file_t read(char *path, uint32_t size);
int write(file_t *file_obj, void *data, int size);
int mount(struct filesystem *fs, char *path);
int list_dir(char *directory, char **output);
int vfs_shutdown();
#endif
