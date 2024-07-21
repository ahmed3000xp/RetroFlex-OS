# Copyright (C) 2024 Ahmed
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

kernel:
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c kernel.c -o build/kernelc.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Headers/util.c -o build/util.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Drivers/VGA/vga.c -o build/vga.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c GDT/gdt.c -o build/gdtc.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c IDT/idt.c -o build/idtc.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Drivers/Speaker/speaker.c -o build/speaker.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Drivers/PIT/pit.c -o build/pit.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Drivers/CMOS/cmos.c -o build/cmos.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Drivers/PS2/ps2.c -o build/ps2.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Headers/multiboot.c -o build/multiboot.o
	gcc -m32 -Werror -ffreestanding -fno-stack-protector -c Paging/paging.c -o build/pagingc.o

	nasm -f elf Boot/boot.asm -o build/boot.o
	nasm -f elf kernel.asm -o build/kernelasm.o
	nasm -f elf GDT/gdt.asm -o build/gdtasm.o
	nasm -f elf IDT/idt.asm -o build/idtasm.o
	nasm -f elf Paging/paging.asm -o build/pagingasm.o

	ld -m elf_i386 -T linker.ld -o build/kernel build/*.o

	mv build/kernel build/iso/boot/kernel

disk:
	grub-mkrescue -o build/os.iso build/iso/

qemu:
	qemu-system-x86_64 -debugcon stdio build/os.iso

clean: build/*.o build/*.iso build/iso/boot/kernel
	rm build/*.o build/*.iso build/iso/boot/kernel