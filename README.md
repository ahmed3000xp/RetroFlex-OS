# RetroFlex OS Project

## Overview
RetroFlex is a UNIX-like OS compatible with GNU+Linux programs and its own programs.

## Installation Instructions

### For Windows Users:
If you are using Windows, you'll need to install WSL (Windows Subsystem for Linux) and then install a GNU+Linux distribution through the Microsoft Store.

### For Debian/Ubuntu/Ubuntu-based Systems:
Run the following command:

sudo apt install xorriso grub-common qemu-system-x86 nasm gcc make -y

For Fedora Systems:

Run the following command:

sudo dnf install -y xorriso nasm qemu grub2 virt-manager virt-viewer dnsmasq vde2 bridge-utils nmap-ncat @development-tools bison flex gmp-devel libmpc-devel mpfr-devel texinfo gcc make

For Arch/Arch-Based Systems:

Run the following command:

sudo pacman -S --noconfirm xorriso nasm qemu grub virt-manager virt-viewer dnsmasq vde2 bridge-utils openbsd-netcat base-devel bison flex gmp libmpc mpfr texinfo gcc make

Yeah, I use Arch, btw.
Compiling and Running RetroFlex OS

After installation, compile and run RetroFlex OS by executing:

make all

Troubleshooting Permissions Issue

If you encounter a permissions error during ISO creation, such as:

rm: remove write-protected regular file '../disk.img'?
...
libburn : SORRY : Failed to open device (a pseudo-drive) : Permission denied
...
qemu-system-x86_64: Could not open '../disk.img': Permission denied

Try running the script with elevated privileges using sudo:

sudo make all

#### Running on x86 Hardware

If you have an x86 based Computer then run the OS on the CPU itself by running:

make qemu_native

License

This project is licensed under the GPL-3.0 license.