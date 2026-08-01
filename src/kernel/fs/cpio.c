#include <drivers/video/vga.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/brainfuck.h>
#include <fs/cpio.h>
#include <fs/vfs.h>
#include <sys/panic.h>
#include <error.h>

typedef struct {
	char c_magic[6];
	char c_ino[8];
	char c_mode[8];
	char c_uid[8];
	char c_gid[8];
	char c_nlink[8];
	char c_mtime[8];
	char c_filesize[8];
	char c_devmajor[8];
	char c_devminor[8];
	char c_rdevmajor[8];
	char c_rdevminor[8];
	char c_namesize[8];
	char c_check[8];
} __attribute__((packed)) cpio_head_t;

uint32_t atoh(const char *s, int len) {
	uint32_t n = 0;
	while (len--) {
		n <<= 4;
		if (*s >= '0' && *s <= '9') {
			n |= *s - '0';
		} else if (*s >= 'A' && *s <= 'F') {
			n |= *s - 'A' + 10;
		} else if (*s >= 'a' && *s <= 'f') {
			n |= *s - 'a' + 10;
		}
		s++;
	}
	return n;
}

static size_t __align_4b(size_t x) { return (x+3) & ~3; }

extern uint8_t _binary_bin_initrd_cpio_start[];
char list_files_cpio() {
	uint8_t *archive = _binary_bin_initrd_cpio_start;
	for(;;) {
		cpio_head_t *header = (cpio_head_t*)archive;
		if (memcmp(header->c_magic, "070701", 6) != 0) return 1; // invalid magic
		int namesize = atoh(header->c_namesize, 8);
		int filesize = atoh(header->c_filesize, 8);
		if (!strcmp((const char *)(archive+110), "TRAILER!!!")) break; // TRAILER!!! is a file in
			// a cpio archive that determines the end-of-archive because
			// __binary_bin_initrd_cpio_end is not always present
		printk(4, "cpio: /%s: namesize %d, filesize %d", archive+110, namesize, filesize);
		archive += 110; // sizeof(cpio_head_t)
		uint8_t *name = archive;
		int header_skip = __align_4b(namesize+110);
		archive -= 110;
		archive += header_skip;
		archive += __align_4b(filesize);
		(void)name; // unused
	}
	return 0;
}

cpio_inode_t read_file_cpio(char *filename) {
	uint8_t *archive = _binary_bin_initrd_cpio_start;
	cpio_inode_t file_inode = {.file = (void*)0, .size = 0};
	if (*filename == '/') filename++;
	for(;;) {
		cpio_head_t *header = (cpio_head_t*)archive;
		if (memcmp(header->c_magic, "070701", 6) != 0) break;
		int namesize = atoh(header->c_namesize, 8);
		int filesize = atoh(header->c_filesize, 8);
		if (!strcmp((const char *)(archive+110), "TRAILER!!!")) break; // TRAILER!!! is a file in
			// the archive that determines EOF
		archive += 110; // sizeof(cpio_head_t)
		uint8_t *name = archive;
		int header_skip = __align_4b(namesize+110);
		archive -= 110;
		archive += header_skip;
		if (!strcmp((const char *)name, filename)) {
			file_inode.file = (void*)archive;
			file_inode.size = filesize;
			break;
		}
		archive += __align_4b(filesize);
	}
	return file_inode;
}

file_t cpio_read(file_t *file_obj_p) {
	file_t file_obj = *file_obj_p;
	file_obj.offset = 0;
	cpio_inode_t cpio_inode = read_file_cpio(file_obj.name);
	file_obj.data = cpio_inode.file;
	file_obj.size = cpio_inode.size;
	return file_obj;
}

int cpio_write(file_t *file_obj, void *data, int size) {
	(void)file_obj; (void)data;
	(void)size;
	return -ENOSUP;
}

int cpio_mount(struct filesystem *cpio_fs) {
	(void)cpio_fs;
	return 0;
}

int cpio_unmount(struct filesystem *cpio_fs) {
	(void)cpio_fs;
	return 0;
}

int cpio_ls(char *directory, char **output) {
	const char **output_old = (const char **)output;
	uint8_t *archive = _binary_bin_initrd_cpio_start;
	if (!directory || !output) return -EBADARG;
	for (;;) {
		cpio_head_t *header = (cpio_head_t*)archive;
		if (memcmp(header->c_magic, "070701", 6) != 0) return -EBAD;
		int namesize = atoh(header->c_namesize, 8);
		int filesize = atoh(header->c_filesize, 8);
		if (!strcmp((const char*)(archive+110), "TRAILER!!!")) break; // EOF file
		archive += 110;
		uint8_t *name = archive;
		int head_skip = __align_4b(namesize+110);
		archive -= 110;
		archive += head_skip;
		*output = (char*)name;
		output++;
		archive += __align_4b(filesize);
	}
	if ((char**)output_old == output) return -ENOFND;
	return 0;
}

static filesystem_t cpio_fs = {};
static struct mount cpio_mountpoint = {};
void init_cpio(void) {
	cpio_fs.read = cpio_read;
	cpio_fs.write = cpio_write;
	cpio_fs.mount = cpio_mount;
	cpio_fs.unmount = cpio_unmount;
	cpio_fs.list_dir = cpio_ls;
	cpio_mountpoint.path[0] = '/';
	cpio_mountpoint.path[1] = 0;
	cpio_mountpoint.fs = &cpio_fs;
	cpio_fs.mountpoint = &cpio_mountpoint;
	printk(4, "read %x write %x mount %x unmount %x", cpio_read, cpio_write, cpio_mount, cpio_unmount);
	printk(4, "path %s fs %x mountpoint %x", &cpio_mountpoint.path, &cpio_fs, &cpio_mountpoint);
	register_fs(&cpio_fs);
	printk(4, "Mounting initramfs at root...");
	int mount_code = mount(&cpio_fs, "/");
	printk(6, "exit code: %d", mount_code);
	if (mount_code != 0) panic("Failed to mount initramfs: %d", mount_code);
	printk(4, "done.");
}
