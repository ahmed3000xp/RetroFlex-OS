RetroFlex OS Installation Instructions
For Windows Users:

If you are using Windows, you'll need to install WSL (Windows Subsystem for Linux) and then install a GNU+Linux distribution through the Microsoft Store.
For Debian/Ubuntu/Ubuntu-based Systems:

Run the following command:

./install-debian.sh

For Fedora Systems:

Run the following command:

./install-fedora.sh

For Arch/Arch-Based Systems:

Run the following command:

./install-arch.sh

Yeah, I use Arch, btw.
Compiling and Running RetroFlex OS

After installation, compile and run RetroFlex OS by executing:

./run.sh

Troubleshooting Permissions Issue

If you encounter a permissions error during ISO creation, such as:

rm: remove write-protected regular file 'build/os.iso'?
mv: replace 'build/iso/boot/kernel', overriding mode 0755 (rwxr-xr-x)?
...
libburn : SORRY : Failed to open device (a pseudo-drive) : Permission denied
...
qemu-system-x86_64: Could not open 'build/os.iso': Permission denied

Try running the script with elevated privileges using sudo:

sudo ./run.sh