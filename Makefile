CCFLAGS = -ffreestanding -m32
CCFLAGSC = -ffreestanding -m32 -fno-exceptions -fno-stack-protector -fno-pie -no-pie -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib
all:
	@echo " MKDIR  build"
	@mkdir build -p
	@echo "    AS  src/boot.s"
	@gcc -c src/boot.s -o build/boot.o $(CCFLAGS)
	@echo "    CC  src/entry.c"
	@gcc -c src/entry.c -o build/entry.o  $(CCFLAGSC)
	@echo "    CC  include/port.c"
	@gcc -c include/port.c -o build/port.o $(CCFLAGSC)
	@echo "    CC  include/kb.c"
	@gcc -c include/kb.c -o build/kb.o $(CCFLAGSC)
	@echo "    LD  build/bootImage.elf"
	@echo "KERNEL: Your kernel is ready"
	@cd build && gcc -T ../kernel.ld *.o -o bootImage.elf $(CCFLAGSC)
	@echo "  COPY  build/bootImage.elf"
	@cp build/bootImage.elf iso
	@echo "  GRUB  build/boot.iso"
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso
clean:
	@echo " CLEAN  build"
	@rm -rf build
	@echo " CLEAN  iso/bootImage.elf"
	@rm -f iso/bootImage.elf
	@echo " CLEAN  build/bootImage.elf"
	@rm -f build/bootImage.elf
	@echo " CLEAN  build/boot.iso"
	@rm -f build/boot.iso
