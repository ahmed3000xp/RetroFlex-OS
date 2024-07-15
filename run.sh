#!/bin/bash

# Copyright (C) 2024 The RetroFlex OS Project
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

echo "RetroFlex OS  Copyright (C) 2024 The Developer"
echo 'This program comes with ABSOLUTELY NO WARRANTY; for details type '\''show w'\''.'
echo 'This is free software, and you are welcome to redistribute it'
echo 'under certain conditions; type '\''show c'\'' for details.'

rm build/*.iso

gcc -m32 -Werror -ffreestanding -c kernel.c -o build/kernelc.o
gcc -m32 -Werror -ffreestanding -c Headers/util.c -o build/util.o
gcc -m32 -Werror -ffreestanding -c Drivers/VGA/vga.c -o build/vga.o
gcc -m32 -Werror -ffreestanding -c GDT/gdt.c -o build/gdtc.o
gcc -m32 -Werror -ffreestanding -c IDT/idt.c -o build/idtc.o
gcc -m32 -Werror -ffreestanding -c Drivers/Speaker/speaker.c -o build/speaker.o
gcc -m32 -Werror -ffreestanding -c Drivers/PIT/pit.c -o build/pit.o
gcc -m32 -Werror -ffreestanding -c Drivers/CMOS/cmos.c -o build/cmos.o
gcc -m32 -Werror -ffreestanding -c Drivers/PS2/ps2.c -o build/ps2.o
gcc -m32 -Werror -ffreestanding -c Headers/multiboot.c -o build/multiboot.o

nasm -f elf Boot/boot.asm -o build/boot.o
nasm -f elf kernel.asm -o build/kernelasm.o
nasm -f elf GDT/gdt.asm -o build/gdtasm.o
nasm -f elf IDT/idt.asm -o build/idtasm.o
nasm -f elf Paging/paging.asm -o build/paging.o

ld -m elf_i386 -T linker.ld -o build/kernel build/boot.o build/kernelc.o build/kernelasm.o build/util.o build/vga.o build/gdtasm.o build/gdtc.o build/idtasm.o build/idtc.o build/speaker.o build/pit.o build/cmos.o build/multiboot.o build/paging.o build/ps2.o

rm build/*.o

mv build/kernel build/iso/boot/kernel

grub-mkrescue -o build/os.iso build/iso/

qemu-system-x86_64 build/os.iso
