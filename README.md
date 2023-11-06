# Background

This Project is Based on [this YouTube tutorial series](https://www.youtube.com/playlist?list=PLZQftyCk7_SeZRitx5MjBKzTtvk0pHMtp) and multiple other Tutorials and Projects like [this](https://github.com/pritamzope/OS/tree/master) or [this one](https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System). It focuses on writing a small x86 Kernel in 64bit Longmode using C++ and adding as much functionality as possible along the way.<br>

The goal of this Project is to learn the basics of Operating Systems, for example how a Kernel works or how to implement Drivers.

For now the OS contains a very barebones version of a Keyboard-Driver that is currently just used to test the Basic Keyboard-Functionality. It only recognises the most basic of keypresses. The Keyboardpresses are hooked up to a System-Interrupt, that can be processed from anywhere in the System. Also the classic C++ IO Operations (std::cout and std::cin) are implmenented and working. The OS also contains a very basic Buddy-Allocator to be able to allocate Memory dynamically.

Sadly due to lack of of time the current state of the project is stuck somewhere in prototyping and testing. Individually the parts seem to work, but for example the IO Operations still use a buffer that has been created using static allocation and the Keyboard-Driver can not yet process keys like Backspace or something else. I hope I soon find the time to clean up here and proceed working on this project. It was quite a lot of fun up to this point.

## Prerequisites

 - A text editor such as [VS Code](https://code.visualstudio.com/).
 - [Docker](https://www.docker.com/) for creating our build-environment.
 - [Qemu](https://www.qemu.org/) for emulating our operating system.
   - Remember to add Qemu to the path so that you can access it from your command-line. ([Windows instructions here](https://dev.to/whaleshark271/using-qemu-on-windows-10-home-edition-4062))
   - `sudo apt install qemu-system-x86` on Debian Based Systems

## Setup

Build an image for our build-environment:
 - `docker build buildenv -t myos-buildenv`

## Build

Enter build environment:
 - Linux or MacOS: `docker run --rm -it -v "$(pwd)":/root/env myos-buildenv`
 - Windows (CMD): `docker run --rm -it -v "%cd%":/root/env myos-buildenv`
 - Windows (PowerShell): `docker run --rm -it -v "${pwd}:/root/env" myos-buildenv`
 - Please use the linux command if you are using `WSL`, `msys2` or `git bash`
 - NOTE: If you are having trouble with an unshared drive, ensure your docker daemon has access to the drive you're development environment is in. For Docker Desktop, this is in "Settings > Shared Drives" or "Settings > Resources > File Sharing".

Build for x86 (other architectures may come in the future):
 - `make build-x86_64`
 - If you are using Qemu, please close it before running this command to prevent errors.

To leave the build environment, enter `exit`.

## Emulate

You can emulate your operating system using [Qemu](https://www.qemu.org/): (Don't forget to [add qemu to your path](https://dev.to/whaleshark271/using-qemu-on-windows-10-home-edition-4062#:~:text=2.-,Add%20Qemu%20path%20to%20environment%20variables%20settings,-Copy%20the%20Qemu)!)

 - `qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso`
 - Note: Close the emulator when finished, so as to not block writing to `kernel.iso` for future builds.

If the above command fails, try one of the following:
 - Windows: [`qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -L "C:\Program Files\qemu"`](https://stackoverflow.com/questions/66266448/qemu-could-not-load-pc-bios-bios-256k-bin)
 - Linux: [`qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -L /usr/share/qemu/`](https://unix.stackexchange.com/questions/134893/cannot-start-kvm-vm-because-missing-bios)
 - Alternatively, install a custom BIOS binary file and link it to Qemu using the `-L` option.

Alternatively, you should be able to load the operating system on a USB drive and boot into it when you turn on your computer. (I haven't actually tested this yet.)

## Cleanup

Remove the build-evironment image:
 - `docker rmi myos-buildenv -f`
