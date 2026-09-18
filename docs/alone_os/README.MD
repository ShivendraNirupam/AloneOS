# AloneOS

> A minimal x86 operating system built from scratch to understand how computers work from the ground up.

**AloneOS** is operating system project focused on learning low-level systems programming, x86 architecture, bootloaders, kernels, memory management, interrupts, and hardware interaction. I started it as a hobby because apparently my sufferings weren't enough to make me suffer.

## Architecture

The current development target is:

```text
x86
 └── 32-bit Protected Mode
      └── C++ Kernel
```

The boot process is being developed from the ground up:

```text
BIOS
  │
  ▼
Bootloader
  │
  ├── Load kernel
  │
  ├── Set up GDT
  │
  └── Enter Protected Mode
          │
          ▼
      C++ Kernel
          │
          ├── Memory Management
          ├── Interrupts
          ├── Drivers
          └── Hardware
```

## Project Structure

```text
AloneOS/
├── CMakeLists.txt
├── README.md
│
├── src/
│   ├── boot/
│   │   └── boot.asm
│   │
│   └── kernel/
│       ├── kernel.cpp
│       └── linker.ld
│
├── include/
│
└── build/
```
The project structure will evolve as more kernel subsystems are implemented.

## Tech Used

* **C++** — Kernel development
* **x86 Assembly** — Bootloader and low-level CPU operations
* **NASM** — Assembly
* **CMake** — Build system
* **i686-elf GCC/G++** — Cross compilation
* **GNU Binutils** — Linking and binary generation
* **QEMU** — Virtual machine and testing

## Building

Configure the project:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Run in QEMU:

```bash
cmake --build build --target run
```


## Long-Term Goals

The long-term goal is to develop a functional operating system with:

* Protected mode / long mode support
* Physical and virtual memory management
* Interrupt handling
* Device drivers
* Preemptive multitasking
* Process isolation
* System calls
* A virtual file system
* User-space programs
* A command-line shell
* Networking
* A custom standard library

## Learning Goals

Through AloneOS, I am exploring:

* Computer architecture
* x86 CPU architecture
* Assembly language
* Boot processes
* Operating system design
* Memory management
* Interrupts and exceptions
* Concurrency and scheduling
* Device drivers
* Compilers and linkers
* ABI and calling conventions
* Low-level C/C++ programming


AloneOS is a learning and research project.

It is **not intended for production use**.

Expect bugs, crashes, triple faults, mysterious reboots, and the occasional moment where the entire machine appears to hate you.

## License

This project is currently intended for educational and experimental purposes.
