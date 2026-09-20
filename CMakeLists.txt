cmake_minimum_required(VERSION 3.20)

# We call nasm / g++ / ld ourselves, so CMake doesn't need to
# detect any compilers. "LANGUAGES NONE" skips that step.
project(AloneOS LANGUAGES NONE)


# ============================================================
# 1. Folders
# ============================================================

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)       # all .o files go here
file(MAKE_DIRECTORY ${OBJ_DIR})


# ============================================================
# 2. Tools
# ============================================================

find_program(NASM nasm                REQUIRED)
find_program(GXX  i686-elf-g++        REQUIRED)
find_program(LD   i686-elf-ld         REQUIRED)
find_program(QEMU qemu-system-x86_64  REQUIRED)
find_program(DD   dd                  REQUIRED)


# ============================================================
# 3. Compiler / assembler flags  (change them in ONE place)
# ============================================================

set(CXX_FLAGS
    -m32
    -std=c++20              # C++20 language features (concepts, auto params, etc.)
    -ffreestanding          # no standard library, no hosted environment
    -fno-exceptions         # no C++ exceptions in a kernel
    -fno-rtti               # no typeid / dynamic_cast
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -fno-threadsafe-statics # static locals won't need __cxa_guard_*
    -O0
    -g
    -Wall
    -Wextra
    -I${SRC_DIR}            # lets you write #include "idt/idt.h"
)

set(ASM_FLAGS
    -f elf32
    -g
)


# ============================================================
# 4. Find source files automatically
#
# CONFIGURE_DEPENDS = when you add or remove a file, CMake
# notices on the next build and re-scans. Nothing to edit here.
# ============================================================

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.asm)
file(GLOB_RECURSE HEADERS     CONFIGURE_DEPENDS ${SRC_DIR}/*.h ${SRC_DIR}/*.inc)

# The bootloader is special: it's a flat binary, not part of the kernel.
set(BOOT_SOURCE ${SRC_DIR}/boot/boot.asm)
list(FILTER ASM_SOURCES EXCLUDE REGEX "/src/boot/")

# kernel.asm holds the kernel entry point (_start), so it MUST be
# linked first. Move it to the front of the list.
set(ENTRY_SOURCE ${SRC_DIR}/kernel.asm)
list(REMOVE_ITEM ASM_SOURCES ${ENTRY_SOURCE})
list(PREPEND     ASM_SOURCES ${ENTRY_SOURCE})


# ============================================================
# 5. Helper: turn a source path into an object file path
#
#   src/idt/idt.cpp  ->  build/obj/idt_idt.cpp.o
#   src/kernel.asm   ->  build/obj/kernel.asm.o
# ============================================================

function(object_path SOURCE OUT_VAR)
    file(RELATIVE_PATH rel ${SRC_DIR} ${SOURCE})
    string(REPLACE "/" "_" rel ${rel})
    set(${OUT_VAR} ${OBJ_DIR}/${rel}.o PARENT_SCOPE)
endfunction()


# ============================================================
# 6. Build rules for every file
# ============================================================

set(KERNEL_OBJECTS "")

# Assembly files first (so kernel.asm ends up first in the link)
foreach(src IN LISTS ASM_SOURCES)
    object_path(${src} obj)

    add_custom_command(
        OUTPUT  ${obj}
        COMMAND ${NASM} ${ASM_FLAGS} ${src} -o ${obj}
        DEPENDS ${src} ${HEADERS}
        COMMENT "ASM  ${src}"
    )

    list(APPEND KERNEL_OBJECTS ${obj})
endforeach()

# Then C++ files
foreach(src IN LISTS CPP_SOURCES)
    object_path(${src} obj)

    add_custom_command(
        OUTPUT  ${obj}
        COMMAND ${GXX} ${CXX_FLAGS} -c ${src} -o ${obj}
        DEPENDS ${src} ${HEADERS}
        COMMENT "C++  ${src}"
    )

    list(APPEND KERNEL_OBJECTS ${obj})
endforeach()


# ============================================================
# 7. Bootloader  ->  boot.bin
# ============================================================

set(BOOT_BIN   ${CMAKE_BINARY_DIR}/boot.bin)
set(KERNEL_BIN ${CMAKE_BINARY_DIR}/kernel.bin)
set(OS_IMAGE   ${CMAKE_BINARY_DIR}/os.bin)

add_custom_command(
    OUTPUT  ${BOOT_BIN}
    COMMAND ${NASM} -f bin ${BOOT_SOURCE} -o ${BOOT_BIN}
    DEPENDS ${BOOT_SOURCE}
    COMMENT "Assembling bootloader"
)


# ============================================================
# 8. Link all objects  ->  kernel.bin
# ============================================================

add_custom_command(
    OUTPUT  ${KERNEL_BIN}
    COMMAND ${LD} -m elf_i386 -T ${SRC_DIR}/linker.ld -o ${KERNEL_BIN} ${KERNEL_OBJECTS}
    DEPENDS ${KERNEL_OBJECTS} ${SRC_DIR}/linker.ld
    COMMENT "Linking kernel"
)


# ============================================================
# 9. Disk image  ->  os.bin
#
#   sector 0      : bootloader
#   sector 1..    : kernel
# ============================================================

add_custom_command(
    OUTPUT ${OS_IMAGE}

    # Empty 1.44 MB floppy image
    COMMAND ${DD} if=/dev/zero     of=${OS_IMAGE} bs=512 count=2880       status=none
    # Bootloader -> sector 0
    COMMAND ${DD} if=${BOOT_BIN}   of=${OS_IMAGE} bs=512 seek=0 conv=notrunc status=none
    # Kernel -> sector 1
    COMMAND ${DD} if=${KERNEL_BIN} of=${OS_IMAGE} bs=512 seek=1 conv=notrunc status=none

    DEPENDS ${BOOT_BIN} ${KERNEL_BIN}
    COMMENT "Creating bootable OS image"
)


# ============================================================
# 10. Targets you actually type
#
#   cmake --build build                 -> builds os.bin
#   cmake --build build --target kernel -> only kernel.bin
#   cmake --build build --target run    -> build + launch QEMU
# ============================================================

add_custom_target(kernel     DEPENDS ${KERNEL_BIN})
add_custom_target(image  ALL DEPENDS ${OS_IMAGE})

add_custom_target(run
    COMMAND ${QEMU} -drive format=raw,file=${OS_IMAGE}
    DEPENDS image
    USES_TERMINAL
)