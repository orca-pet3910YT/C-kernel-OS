CCFLAGS = -ffreestanding -m32
CCFLAGSC = -ffreestanding -m32 -fno-exceptions -fno-stack-protector -fno-pie -no-pie -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib -Wall -Wextra
all:
	@echo " MKDIR  build"
	@mkdir build -p
	@echo "    AS  src/boot.s"
	@gcc -c src/boot.s -o build/boot.o $(CCFLAGS)
	@echo "    CC  src/entry.c"
	@gcc -c src/entry.c -o build/entry.o $(CCFLAGSC)
	@echo "    CC  include/serial.c"
	@gcc -c include/serial.c -o build/serial.o $(CCFLAGSC)
	@echo "    CC  include/port.c"
	@gcc -c include/port.c -o build/port.o $(CCFLAGSC)
	@echo "    CC  include/kb.c"
	@gcc -c include/kb.c -o build/kb.o $(CCFLAGSC)
	@echo "    RM  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo "    LD  build/bootImage.elf"
	@echo "KERNEL: Your kernel is ready"
	@cd build && gcc -T ../kernel.ld *.o -o bootImage.elf $(CCFLAGSC)
	@echo "  COPY  build/bootImage.elf"
	@cp build/bootImage.elf iso
	@echo "  GRUB  build/boot.iso"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso
	@make qemu

qemu-g:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -serial stdio

qemu:
	@echo "  QEMU  build/boot.iso"
	@qemu-system-i386 -cdrom build/boot.iso -nographic

clean:
	@echo " CLEAN  build"
	@rm -rf build
	@echo " CLEAN  iso/bootImage.elf"
	@rm -f iso/bootImage.elf
	@echo " CLEAN  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo " CLEAN  build/boot.iso"
	@rm -f build/boot.iso
