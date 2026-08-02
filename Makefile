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
	   -msse \
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
MINOR = 06
PATCH = 0
ADDITIONAL = $(empty)

#.PHONY: build

all: build build/boot.iso.gz

build:
	@echo "    RUN  scripts/build_inc.sh"; scripts/build_inc.sh
	@echo "    RUN  scripts/check_dirs.sh"; scripts/check_dirs.sh
	@echo "    RUN  scripts/gen_ver.sh"; scripts/gen_ver.sh
	@echo "    RUN  scripts/gen_ver_ex.sh"; scripts/gen_ver_ex.sh

include/generated/config.h: .config | build
	@echo "   MAKE  make.gen"; MAJOR=$(MAJOR) MINOR=$(MINOR) PATCH=$(PATCH) ADDITIONAL=$(ADDITIONAL) $(MAKE) -f make.gen --no-print-directory

build/src/%.c.o: src/%.c include/generated/config.h | build
	@mkdir -p $(dir $@)
	@echo "     CC  $<"
	@clang -c $< -o $@ $(CCFLAGSC)

build/src/%.s.o: src/%.s | build
	@mkdir -p $(dir $@)
	@echo "     AS  $<"
	@clang -c $< -o $@ $(CCFLAGS)

build/src/%.asm.o: src/%.asm | build
	@mkdir -p $(dir $@)
	@echo "   NASM  $<"
	@nasm -f elf32 $< -o $@

build/font_file.o: include/generated/config.h
build/logo.o: include/generated/config.h
initrd: build/initrd.o
build/initrd.o: include/generated/config.h

kernel: build/bootImage.elf
build/bootImage.elf: $(OBJECTS)
	@echo "    LLD  build/bootImage.unstripped.elf"; ld.lld -m elf_i386 -T kernel.ld $(OBJECTS) -o build/bootImage.unstripped.elf
	@echo "  STRIP  build/bootImage.elf"; $(CROSS)strip -s build/bootImage.unstripped.elf -o build/bootImage.elf

iso: build/boot.iso
build/boot.iso: build/bootImage.elf
	@cp build/bootImage.elf build/bootImage.unstripped.elf iso
	@echo "    ISO  build/boot.iso"; grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso > /dev/null 2> /dev/null
isogz: build/boot.iso.gz
build/boot.iso.gz: build/boot.iso
	@echo "   GZIP  build/boot.iso.gz"; gzip -f9k build/boot.iso

clean:
	@echo "  CLEAN  build"; rm -rf build
	@echo "  CLEAN  iso/*.elf"; rm -f iso/*.elf
	@echo "  CLEAN  include/generated"; rm -rf include/generated

run:
	@echo "Running in QEMU"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -nographic -cpu max

run-vnc:
	@echo "Running in QEMU (VNC 1)"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial stdio -display vnc=:0 -d int -cpu max

run-debug:
	@echo "Running in QEMU (debugged)"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -cpu max -s -S

menuconfig:
	@kconfig-mconf Kconfig

allyesconfig:
	@kconfig-conf --allyesconfig Kconfig

allnoconfig:
	@kconfig-conf --allnoconfig Kconfig

mrproper:
	@echo "  CLEAN  build"; rm -rf build
	@echo "  CLEAN  iso/*.elf"; rm -f iso/*.elf
	@echo "  CLEAN  include/generated"; rm -rf include/generated
	@echo "  CLEAN  .version"; echo -n "0" > .version
