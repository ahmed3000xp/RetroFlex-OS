# RetroFlex OS Project

## Overview
RetroFlex is a UNIX-like OS compatible with GNU+Linux programs and its own programs.

## Installation Instructions

### For Windows Users:
If you are using Windows, you'll need to install WSL (Windows Subsystem for Linux) and then install a GNU+Linux distribution through the Microsoft Store.

### For Debian/Ubuntu/Ubuntu-based Systems:
Run the following command:

sudo apt install grub2 qemu-system-x86 nasm gcc make parted dosfstools

### For Fedora Systems:

Run the following command:

sudo dnf install nasm qemu grub2 virt-manager virt-viewer dnsmasq vde2 bridge-utils nmap-ncat @development-tools bison flex gmp-devel libmpc-devel mpfr-devel texinfo gcc make parted dosfstools

### For Arch/Arch-Based Systems:

Run the following command:

sudo pacman -Syy nasm grub virt-manager virt-viewer dnsmasq vde2 bridge-utils openbsd-netcat base-devel bison flex gmp libmpc mpfr texinfo gcc make qemu-full parted dosfstools

### Compiling and Running RetroFlex OS

After installation, compile and run RetroFlex OS by executing:

make all

### Troubleshooting Permissions Issue

If you encounter a permissions error during ISO creation, such as:

rm: remove write-protected regular file '../disk.img'?
...
qemu-system: Could not open '../disk.img': Permission denied

Try running the script with elevated privileges using sudo:

sudo make all

#### Running on x86 Hardware

If you have an x86 based Computer then run the OS on the CPU itself by running:

make qemu_native

### Fun Facts

- **Did you know?** RetroFlex OS was created by a furry who is also an Arch user. He claims his choice of operating systems is as eclectic as his sense of humor!

- **Quirky Code:** The developer once tried to program RetroFlex OS while juggling flaming torches. Spoiler: It didn't end well, but the OS survived!

- **Unlikely Inspiration:** The idea for RetroFlex OS came during a late-night snack attack when the developer mistook a pizza box for a new motherboard. Pizza might not be the best hardware, but it sure inspired some great software!

- **Arch Enthusiasm:** It’s rumored that the developer’s love for Arch GNU+Linux is so strong that they even use Arch stickers to decorate their workspace. Rumor also has it that the stickers are more organized than their code!

- **Furry Fact:** The developer claims that RetroFlex OS runs smoother when you pet your favorite plushie while compiling. It's unproven, but it might just be the secret to compiling success!

#### License

This project is licensed under the GPL-3.0 license.

#### Contact

For support or questions, please contact me at ahmed.gamer.stupid@proton.me.
