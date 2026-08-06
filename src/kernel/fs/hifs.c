/*
 * a filesystem for testing mountpoints and VFS functions
 * returns exactly one file /hi containing "hi\n", or at least should
 */

#include <generated/config.h>
#if CONFIG_HIFS
#include <fs/vfs.h>
#include <string.h>
#include <error.h>
// #include <drivers/video/vga.h>

static file_t hifs_read(file_t *file_read) {
	if (strcmp(file_read->name, "/hi") == 0) {
		file_read->offset = 0;
		file_read->data = "hi\n";
		file_read->size = 3;
		return *file_read;
	}
	return null_file;
}

static int hifs_write(file_t *file_write, void *data, int size) {
	(void)data; (void)file_write; (void)size;
	return -EBADARG;
}

static int hifs_mount(struct filesystem *fs) {
	(void)fs; return 0;
}

static int hifs_unmount(struct filesystem *fs) {
	(void)fs; return 0;
}

static int hifs_list_dir(char *path, char **output) {
	if (strcmp(path, "/") == 0) {
		output[0] = "hi";
		return 0;
	}
	return -ENOFILE;
}

static filesystem_t hifs_fs;
//static mount_t hifs_mount;
void hifs_init() {
	hifs_fs.read = hifs_read;
	hifs_fs.write = hifs_write;
	hifs_fs.mount = hifs_mount;
	hifs_fs.unmount = hifs_unmount;
	hifs_fs.list_dir = hifs_list_dir;
	//strcpy("/hifs", hifs_mount.path);
	//hifs_fs.mountpoint = hifs_mount;
	//hifs_mount.fs = hifs_fs;
	register_fs(&hifs_fs);
	mount(&hifs_fs, "/hifs");
}
#else
void hifs_init() {}
#endif
