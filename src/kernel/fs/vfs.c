#include <fs/vfs.h>
#include <stdint.h>
#include <string.h>
#include <drivers/video/vga.h>
#include <error.h>
#include <sys/globals.h>

#define MAX_FS 4
#define MAX_MOUNT 8
#define MAX_FD 256

static filesystem_t filesystems[MAX_FS];
static int fs_count = 0;
static mount_t mounts[MAX_MOUNT];
static int mount_count = 0;
// for the future
//static file_t fds[MAX_FD];
//static int fd_count = 0;
static char root_str[256] = "/";

char *resolve_path(char *path, char *cwd) {
	if (*path == '/') return path;
	int cwd_len = strlen(cwd);
	char *cwd_use = cwd;
	if (cwd_use[cwd_len] != '/') cwd_use = strcat(cwd_use, "/");
	path = strcat(cwd_use, path);
	return path;
}

int register_fs(filesystem_t *fs) {
	int free_fs_index = 0;
	if (fs->unmount && fs->mount && fs->write && fs->read) { // validate filesystem object
		for (; free_fs_index < MAX_FS; free_fs_index++) if (!filesystems[free_fs_index].read) break;
		if (free_fs_index == MAX_FS-1) {
			printk(4, "vfs: error: out of filesystem spots to register. (-%d)", EUNAVAIL);
			return -EUNAVAIL;
		}
		filesystems[free_fs_index] = *fs;
		fs_count++;
	}
	return 0;
}

int unregister_fs(filesystem_t *fs) {
	int fs_index = 0;
	for (; fs_index < MAX_FS && filesystems[fs_index].read == fs->read
			&& filesystems[fs_index].write == fs->write
			&& filesystems[fs_index].mount == fs->mount
			&& filesystems[fs_index].unmount == fs->unmount; fs_index++);
	if (fs_index == MAX_MOUNT-1) {
		printk(2, "vfs: error: failed to unregister fs %s. (-%d)", &fs->mountpoint->path, ENOFND);
		return -ENOFND;
	}
	filesystems[fs_index].read = 0;
	filesystems[fs_index].write = 0;
	filesystems[fs_index].mount = 0;
	filesystems[fs_index].unmount = 0;
	filesystems[fs_index].mountpoint = 0;
	return 0;
}

int mount(struct filesystem *fs, char *path) {
	mount_t mountpoint;
	int free_mount = 0;
	if (fs->read && fs->write && fs->mount && fs->unmount) {
		mountpoint.fs = fs;
		if (strlen(path) > 255) {
			printk(2, "vfs: error: mountpoint path too large. %d > 255 (-%d)", strlen(path), ETOOBIG);
			return -ETOOBIG;
		}
		strcpy(mountpoint.path, path);
		for (; free_mount < MAX_MOUNT && mounts[free_mount].path[0]; free_mount++);
		if (free_mount == MAX_MOUNT-1) {
			printk(2, "vfs: error: out of mountpoints. (-%d)", EUNAVAIL);
			return -EUNAVAIL;
		}
		mounts[mount_count] = mountpoint;
		fs->mountpoint = &mounts[mount_count++];
		return fs->mount(fs);
	}
	return EGENERIC;
}

int unmount(char *path) {
	for (int i = 0; i < MAX_MOUNT; i++) {
		if (!strncmp(mounts[i].path, path, 256) && mounts[i].fs) {
			int ret_value = mounts[i].fs->unmount(mounts[i].fs);
			memset(mounts[i].path, 0, 256);
			mounts[i].fs = (struct filesystem*)0;
			return ret_value;
		}
	}
	return 0;
}

int vfs_shutdown() {
	for (int i = 0; i < MAX_MOUNT; i++) {
		if (mounts[i].path[0] && mounts[i].fs) {
			printk(4, "vfs: unmounting fs %d, fs returned %d", i, mounts[i].fs->unmount(mounts[i].fs));
			memset(mounts[i].path, 0, 256);
			mounts[i].fs = (struct filesystem*)0;
		}
	}
	return 0;
}

file_t read(char *path, uint32_t size) {
	file_t file_obj = {0};
	file_obj.offset = 0;
	for (int i = 0; i < MAX_MOUNT; i++) {
		if (mounts[i].fs->read) {
			if (strncmp(path, mounts[i].path, strlen(mounts[i].path)-1) != 0) continue;
			//char *filepath = path;
			//filepath += strlen(mounts[i].path);
			//if (*filepath != '/') filepath = strcat(root_str, filepath);
			//file_obj.name = filepath;
			char filepath[256];
			strcpy(filepath, path+strlen(mounts[i].path));
			if (*filepath != '/') {
				char tmp[256] = "/";
				strcat(tmp, filepath);
				strcpy(filepath, tmp);
			}
			strncpy(file_obj.name, filepath, sizeof(file_obj.name)-1);
			file_obj.name[sizeof(file_obj.name)-1] = 0;
			file_obj.size = size;
			file_t file_read = mounts[i].fs->read(&file_obj);
			if (file_read.offset == 0 && file_read.data == (void*)0 && file_read.size == 0 && file_read.name == (char*)0) continue;
			for (int j = 0; j < (int)sizeof(root_str); j++) root_str[j] = 0;
			return file_read;
		}
	}
	return file_obj;
}

int write(file_t *file_obj, void *data, int size) {
	for (int i = 0; i < mount_count; i++) {
		if (mounts[i].fs->write) {
			char *filepath = file_obj->name;
			filepath += strlen(mounts[i].path);
			if (*filepath != '/') filepath = strcat(root_str, filepath);
			int file_write = mounts[i].fs->write(file_obj, data, size);
			for (int j = 0; j < (int)sizeof(root_str); j++) root_str[j] = 0;
			return file_write;
		}
	}
	return 1;
}

int list_dir(char *directory, char **output) {
	if (!directory || !output) return -EBADARG;
	for (int i = 0; i < MAX_MOUNT; i++) {
		if (mounts[i].fs->list_dir) {
			if (strncmp(directory, mounts[i].path, strlen(mounts[i].path)-1) != 0) continue;
			return mounts[i].fs->list_dir(directory, output);
		}
	}
	return -ENOFND;
}
