CCFLAGS = \
 -target i386-elf \
 -fno-pie -fno-pic \
 -Wunused -Wall -Wextra \
 -Iinclude/lib -Iinclude/kernel

CCFLAGSC = -ffreestanding \
           -target i386-elf \
	   -fno-exceptions \
	   -fno-stack-protector \
	   -fno-align-functions \
	   -fno-pie \
	   -fno-pic \
	   -fno-unwind-tables \
	   -fno-asynchronous-unwind-tables \
	   -fno-omit-frame-pointer \
	   -fno-optimize-sibling-calls \
	   -I include \
	   -I include/kernel \
	   -I include/lib \
	   -nostdlib -Wall -Wextra -fno-ident -Wunused -O3 -msse2 -g
SRC_C := $(shell find src -name '*.c')
SRC_S := $(shell find src -name '*.s')
SRC_ASM := $(shell find src -name '*.asm')
OBJECTS := $(patsubst src/%.c,build/src/%.c.o,$(SRC_C)) \
		$(patsubst src/%.s,build/src/%.s.o,$(SRC_S)) \
		$(patsubst src/%.asm,build/src/%.asm.o,$(SRC_ASM)) build/font_file.o build/logo.o build/initrd.o
OBJECTS := $(shell shuf -e $(OBJECTS) | tr '\n' ' ')

empty :=

MAJOR = 0
MINOR = 05
PATCH = 0
ADDITIONAL = $(empty)

#.PHONY: build

all: build build/boot.iso.gz

.version:
	@echo "Running scripts/build_inc.sh"
	@scripts/build_inc.sh

build: .version
	@echo "Create build/ subdirectories"
	@mkdir -p build/src/dev build/src/kernel build/src/stdlib
	@echo "Create include/generated/"
	@mkdir -p include/generated
	@echo "Setting script permissions"
	@chmod +x scripts/*.sh
	@echo "Running scripts/check_dirs.sh"
	@scripts/check_dirs.sh
	@echo "Running scripts/gen_ver.sh"
	@scripts/gen_ver.sh
	@echo "Running scripts/gen_ver_ex.sh"
	@scripts/gen_ver_ex.sh

include/generated/config.h: .config | build
	@echo "Config has been changed. Regenerating"
	@scripts/gen_conf.sh $(MAJOR) $(MINOR) $(PATCH) "$(ADDITIONAL)"
	@echo "Config regenerated"

build/src/%.c.o: src/%.c include/generated/config.h | build
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@clang -c $< -o $@ $(CCFLAGSC)

build/src/%.s.o: src/%.s | build
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@clang -c $< -o $@ $(CCFLAGS)

build/src/%.asm.o: src/%.asm | build
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@nasm -f elf32 $< -o $@

build/font_file.o: bin/default_8x16.psf | build
	@echo "Creating font object"
	@ld.lld -r -b binary $< -o $@ -m elf_i386

build/logo.o: bin/logo.raw | build
	@echo "Building logo object"
	@$(CROSS)objcopy -I binary -O elf32-i386 bin/logo.raw build/logo.o

initrd: build/initrd.o
build/initrd.o: bin/initrd.cpio | build
	@echo "Building initramfs"
	@$(CROSS)objcopy -I binary -O elf32-i386 bin/initrd.cpio build/initrd.o

kernel: build/bootImage.elf
build/bootImage.elf: $(OBJECTS)
	@echo "Linking the kernel"
	@ld.lld -m elf_i386 -T kernel.ld $(OBJECTS) -o build/bootImage.unstripped.elf
	@echo "Stripping the kernel"
	@$(CROSS)strip -s build/bootImage.unstripped.elf -o build/bootImage.elf

iso: build/boot.iso
build/boot.iso: build/bootImage.elf
	@echo "Copying kernels"
	@cp build/bootImage.elf build/bootImage.unstripped.elf iso
	@echo "Make bootable ISO"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso
isogz: build/boot.iso.gz
build/boot.iso.gz: build/boot.iso
	@echo "Compressing ISO for distribution"
	@gzip -f9k build/boot.iso

clean:
	@echo "Cleaning..."
	@rm -rf build iso/bootImage.elf include/generated/*.h

run: build/boot.iso
	@echo "Running in QEMU"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial mon:vc -serial stdio -vga std -global VGA.vgamem_mb=128 -cpu max

run-vnc: build/boot.iso
	@echo "Running in QEMU (VNC 1)"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial stdio -display vnc=:0 -d int -cpu max

run-debug: build/boot.iso
	@echo "Running in QEMU (debugged)"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial mon:vc -serial stdio -vga std -global VGA.vgamem_mb=128 -cpu max -s -S -display sdl

menuconfig:
	@kconfig-mconf Kconfig

allyesconfig:
	@kconfig-conf --allyesconfig Kconfig

allnoconfig:
	@kconfig-conf --allnoconfig Kconfig

mrproper:
	@echo "Erasing all data!"
	@rm -rf build iso/bootImage.elf include/generated/*.h .version
