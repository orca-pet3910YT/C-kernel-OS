CCFLAGS = -target i386-elf -fno-pie -fno-pic
CCFLAGSC = -ffreestanding -target i386-elf -fno-exceptions -fno-stack-protector -fno-pie -fno-pic -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib -Wall -Wextra -fno-ident
SRC_C := $(wildcard src/*.c)
SRC_S := $(wildcard src/*.s)
SRC_ASM := $(wildcard src/*.asm)
OBJECTS := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c)) $(patsubst src/%.s,build/%.o,$(wildcard src/*.s)) $(patsubst src/%.asm,build/%.o,$(SRC_ASM))

.NOTPARALLEL:

all: build/boot.iso

build:
	@echo "Create build/"
	@mkdir -p build
	@echo "Create include/generated/"
	@mkdir -p include/generated
	@echo "Running scripts/gen_ver.sh"
	@scripts/gen_ver.sh

build/%.o: src/%.c | build
	@echo "Compiling $<"
	@clang -c $< -o $@ $(CCFLAGSC)

build/%.o: src/%.s | build
	@echo "Assembling $<"
	@clang -c $< -o $@ $(CCFLAGS)

build/%.o: src/%.asm | build
	@echo "Assembling $<"
	@nasm -f elf32 $< -o $@

build/bootImage.elf: $(OBJECTS)
	@echo "Linking the kernel"
	@ld.lld -m elf_i386 -T kernel.ld $(OBJECTS) -o build/bootImage.elf

build/boot.iso: build/bootImage.elf
	@echo "Copying kernel"
	@cp build/bootImage.elf iso
	@echo "Make bootable ISO"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso

clean:
	@echo "Cleaning..."
	@rm -rf build iso/bootImage.elf include/generated

qemu:
	@echo "Running in QEMU"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial stdio

cl:
	@echo -n "lines of code in total: "
	@grep -R "" src/*.c src/*.s src/*.asm include/*.h Makefile kernel.ld iso/boot/grub/grub.cfg | wc -l
