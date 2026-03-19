CCFLAGS = -target i386-elf -fno-pie -fno-pic
CCFLAGSC = -ffreestanding -target i386-elf -fno-exceptions -fno-stack-protector -fno-pie -fno-pic -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib -Wall -Wextra -fno-ident
.NOTPARALLEL:

all:
	@echo "Running scripts/gen_ver.sh"
	@bash scripts/gen_ver.sh
	@echo " MKDIR  build"
	@mkdir build -p
	@echo "    AS  src/boot.s"
	@clang -c src/boot.s -o build/boot.o $(CCFLAGS)
	@echo "  NASM  src/idtld.s"
	@nasm -f elf32 src/idtld.s -o build/idtld.o
	@echo "    CC  src/idt.c"
	@clang -c src/idt.c -o build/idt.o $(CCFLAGSC)
	@echo "    CC  src/vga.c"
	@clang -c src/vga.c -o build/vga.o $(CCFLAGSC)
	@echo "    CC  src/entry.c"
	@clang -c src/entry.c -o build/entry.o $(CCFLAGSC)
	@echo "    CC  src/stdlib.c"
	@clang -c src/stdlib.c -o build/stdlib.o $(CCFLAGSC)
	@echo "    CC  src/serial.c"
	@clang -c src/serial.c -o build/serial.o $(CCFLAGSC)
	@echo "    CC  src/port.c"
	@clang -c src/port.c -o build/port.o $(CCFLAGSC)
	@echo "    CC  src/kb.c"
	@clang -c src/kb.c -o build/kb.o $(CCFLAGSC)
	@echo "    CC  src/string.c"
	@clang -c src/string.c -o build/string.o $(CCFLAGSC)
	@echo "    CC  src/globals.c"
	@clang -c src/globals.c -o build/globals.o $(CCFLAGSC)
	@echo "    CC  src/power.c"
	@clang -c src/power.c -o build/power.o $(CCFLAGSC)
	@echo "    CC  src/panic.c"
	@clang -c src/panic.c -o build/panic.o $(CCFLAGSC)
	@echo "  NASM  src/gdtf.s"
	@nasm -f elf32 src/gdtf.s -o build/gdtf.o
	@echo "    CC  src/gdt.c"
	@clang -c src/gdt.c -o build/gdt.o $(CCFLAGSC)
	@echo "    CC  src/pit.c"
	@clang -c src/pit.c -o build/pit.o $(CCFLAGSC)
	@echo "    CC  src/pic.c"
	@clang -c src/pic.c -o build/pic.o $(CCFLAGSC)
	@echo "    RM  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo "    LD  build/bootImage.elf"
	@echo "KERNEL: Your kernel is ready"
	@cd build && ld.lld -m elf_i386 -T ../kernel.ld *.o -o bootImage.elf # clang -fuse-ld=lld -T ../kernel.ld *.o -o bootImage.elf -target i386-elf -ffreestanding -nostdlib -no-pie -Wl,--build-id=none -m32
	@echo "  COPY  build/bootImage.elf"
	@cp build/bootImage.elf iso
	@echo "  GRUB  build/boot.iso"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso

qemu-g:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -boot order=dca -nic none -serial stdio

qemu:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -nographic -boot order=dca -nic none

cl:
	@echo -n "lines of code in total: "
	@grep -R "" src/*.c src/*.s include/*.h Makefile kernel.ld iso/boot/grub/grub.cfg | wc -l

clean:
	@echo " CLEAN  build"
	@rm -rf build
	@echo " CLEAN  iso/bootImage.elf"
	@rm -f iso/bootImage.elf
	@echo " CLEAN  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo " CLEAN  build/boot.iso"
	@rm -f build/boot.iso
