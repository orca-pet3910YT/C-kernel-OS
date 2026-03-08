CCFLAGS = -ffreestanding -m32
CCFLAGSC = -ffreestanding -m32 -fno-exceptions -fno-stack-protector -fno-pie -no-pie -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib -Wall -Wextra -fno-ident -Wl,--build-id=none
all:
	@echo " MKDIR  build"
	@mkdir build -p
	@echo "    AS  src/boot.s"
	@gcc -c src/boot.s -o build/boot.o $(CCFLAGS)
	@echo "    CC  src/vga.c"
	@gcc -c src/vga.c -o build/vga.o $(CCFLAGSC)
	@echo "    CC  src/entry.c"
	@gcc -c src/entry.c -o build/entry.o $(CCFLAGSC)
	@echo "    CC  src/stdlib.c"
	@gcc -c src/stdlib.c -o build/stdlib.o $(CCFLAGSC)
	@echo "    CC  src/serial.c"
	@gcc -c src/serial.c -o build/serial.o $(CCFLAGSC)
	@echo "    CC  src/port.c"
	@gcc -c src/port.c -o build/port.o $(CCFLAGSC)
	@echo "    CC  src/kb.c"
	@gcc -c src/kb.c -o build/kb.o $(CCFLAGSC)
	@echo "    CC  src/string.c"
	@gcc -c src/string.c -o build/string.o $(CCFLAGSC)
	@echo "    CC  src/globals.c"
	@gcc -c src/globals.c -o build/globals.o $(CCFLAGSC)
	@echo "    RM  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo "    LD  build/bootImage.elf"
	@echo "KERNEL: Your kernel is ready"
	@cd build && gcc -T ../kernel.ld *.o -o bootImage.elf $(CCFLAGSC)
	@echo "  COPY  build/bootImage.elf"
	@cp build/bootImage.elf iso
	@echo "  GRUB  build/boot.iso"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso
	@make qemu-g

qemu-g:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -serial stdio -boot order=dca -nic none -cpu max

qemu:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -nographic -boot order=dca -nic none

cl:
	@echo -n "lines of code in total: "
	@grep -R "" src/*.* include/*.* Makefile kernel.ld iso/boot/grub/grub.cfg | wc -l

clean:
	@echo " CLEAN  build"
	@rm -rf build
	@echo " CLEAN  iso/bootImage.elf"
	@rm -f iso/bootImage.elf
	@echo " CLEAN  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo " CLEAN  build/boot.iso"
	@rm -f build/boot.iso
