CCFLAGS = -ffreestanding -m32
CCFLAGSC = -ffreestanding -m32 -fno-exceptions -fno-stack-protector -fno-pie -no-pie -fno-unwind-tables -fno-asynchronous-unwind-tables -I include -nostdlib
all:
	mkdir build -p
	gcc -c src/boot.s -o build/boot.o $(CCFLAGS)
	gcc -c src/entry.c -o build/entry.o  $(CCFLAGSC)
	gcc -c include/port.c -o build/port.o $(CCFLAGSC)
	gcc -c include/kb.c -o build/kb.o $(CCFLAGSC)
	cd build && gcc -T ../kernel.ld *.o -o bootImage.elf $(CCFLAGSC)
	cp build/bootImage.elf iso
	grub-mkrescue -d /usr/lib/grub/i386-pc -o build/boot.iso iso

