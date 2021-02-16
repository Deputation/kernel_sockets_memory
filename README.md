# kernel_sockets_memory 💻
Kernel-based memory hacking framework communicating with a kernel driver via sockets.
Kernel-mode sockets kindly provided by wbenny's KSOCKET framework. Credits to him. https://github.com/wbenny/ksocket

# Notes
The driver was written entirely in C, while the client was written in C++. The part of the project meant to be used by people who have no idea how it works behind the scenes (the ``memory::virtual_controller_t`` class) is well documented.

# Loading the driver
The driver can be loaded regularly and also via DSE-bypass techniques like manual mapping, as it doesn't use any ``__try{} __except() {}`` blocks nor tries doing anything patchguard-unfriendly. Once loaded, the driver spawns a new thread for every client that would like to connect.

# Client
The client connects and sends packets with the regular wsa socket api present in windows. The idea to use unions for the socket structures was taken from r6s-external-nuklear-socket by alxbrn. Credits to him. https://github.com/alxbrn/r6s-external-nuklear-socket

# Tests
A small subset of "tests" showing simply how to use the provided ``memory::virtual_controller_t`` class is present in the main() function of the client.

# Getting started
- Download VS2019 with the relevant C++ packages and the W10 SDK.
- Download the Windows WDK and install its relevant extension.
- Clone the project.
- Build.

# How to use
- Create the driver's service by executing "``sc create ksocketsmem type= kernel binPath= "C:\Path\To\Your\File.sys"``" in an elevated command prompt.
- Start the driver's service by executing "``sc start ksocketsmem``" in an elevated command prompt.
- Open the client.
