If you are using windows then you need to install WSL and then install a GNU+Linux distro through the MS store

If you're on a Debian/Ubuntu/Ubuntu-based system run ./install-debian.sh
If you're on a Fedora system run ./install-fedora.sh
If you're on a Arch/Arch-Based system run ./install-arch.sh I don't use Arch, btw
run ./run.sh to compile then run the OS but if it gave you this "RetroFlex OS  Copyright (C) 2024 The Developer
This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type 'show c' for details.
rm: remove write-protected regular file 'build/os.iso'? 
mv: replace 'build/iso/boot/kernel', overriding mode 0755 (rwxr-xr-x)? 
xorriso 1.5.4 : RockRidge filesystem manipulator, libburnia project.

Drive current: -outdev 'stdio:build/os.iso'
Media current: stdio file, overwriteable
Media status : is blank
Media summary: 0 sessions, 0 data blocks, 0 data,  134g free
Added to ISO image: directory '/'='/tmp/grub.yCChws'
xorriso : UPDATE :     667 files added in 1 seconds
Added to ISO image: directory '/'='/media/ahmed/HDD/home/OS32/build/iso'
xorriso : UPDATE :     671 files added in 1 seconds
xorriso : NOTE : Copying to System Area: 512 bytes from file '/usr/lib/grub/i386-pc/boot_hybrid.img'
xorriso : UPDATE : Thank you for being patient. Working since 0 seconds.
libburn : SORRY : Failed to open device (a pseudo-drive) : Permission denied
xorriso : UPDATE : Thank you for being patient. Working since 1 seconds.
libburn : FATAL : Burn run failed
xorriso : FATAL : -abort_on 'FAILURE' encountered 'FATAL' during image writing
xorriso : NOTE : libburn has now been urged to cancel its operation
xorriso : UPDATE : Thank you for being patient. Working since 2 seconds.
xorriso : UPDATE : Thank you for being patient. Working since 3 seconds.
xorriso : UPDATE : Thank you for being patient. Working since 4 seconds.
libisofs: MISHAP : Image write cancelled
xorriso : FAILURE : libburn indicates failure with writing.
xorriso : NOTE : -return_with SORRY 32 triggered by problem severity FATAL
grub-mkrescue: error: `xorriso` invocation failed
.
WARNING: Image format was not specified for 'build/os.iso' and probing guessed raw.
         Automatically detecting the format is dangerous for raw images, write operations on block 0 will be restricted.
         Specify the 'raw' format explicitly to remove the restrictions.
qemu-system-x86_64: Could not open 'build/os.iso': Permission denied" then it maybe needs sudo so run sudo ./run.sh
