cmake_minimum_required(VERSION 3.16)

project(AloneOS ASM_NASM)

set(BUILD_DIR ${CMAKE_BINARY_DIR})


set(BOOT_SRC
    ${CMAKE_SOURCE_DIR}/src/boot/boot.asm
)

set(KERNEL_CPP
    ${CMAKE_SOURCE_DIR}/src/kernel.cpp
)

set(KERNEL_ASM
    ${CMAKE_SOURCE_DIR}/src/kernel.asm
)

set(LINKER_SCRIPT
    ${CMAKE_SOURCE_DIR}/src/linker.ld
)

set(BOOT_BIN
    ${BUILD_DIR}/boot.bin
)

set(KERNEL_CPP_OBJ
    ${BUILD_DIR}/kernel_cpp.o
)

set(KERNEL_ASM_OBJ
    ${BUILD_DIR}/kernel_asm.o
)

set(KERNEL_BIN
    ${BUILD_DIR}/kernel.bin
)

set(OS_IMAGE
    ${BUILD_DIR}/os.bin
)


# target_include_directories()


find_program(NASM_EXECUTABLE nasm REQUIRED)

find_program(I686_GXX_EXECUTABLE
    i686-elf-g++
    REQUIRED
)

find_program(I686_LD_EXECUTABLE
    i686-elf-ld
    REQUIRED
)

find_program(QEMU_EXECUTABLE
    qemu-system-x86_64
    REQUIRED
)



add_custom_command(
    OUTPUT ${BOOT_BIN}

    COMMAND ${NASM_EXECUTABLE}
            -f bin
            ${BOOT_SRC}
            -o ${BOOT_BIN}

    DEPENDS ${BOOT_SRC}

    COMMENT "Assembling bootloader"
)

add_custom_target(boot ALL
    DEPENDS ${BOOT_BIN}
)


add_custom_command(
    OUTPUT ${KERNEL_CPP_OBJ}

    COMMAND ${I686_GXX_EXECUTABLE}
            -m32
            -ffreestanding
            -fno-exceptions
            -fno-rtti
            -fno-stack-protector
            -fno-pic
            -fno-pie
            -c
            ${KERNEL_CPP}
            -o ${KERNEL_CPP_OBJ}

    DEPENDS ${KERNEL_CPP}

    COMMENT "Compiling kernel C++"
)



add_custom_command(
    OUTPUT ${KERNEL_ASM_OBJ}

    COMMAND ${NASM_EXECUTABLE}
            -f elf32
            ${KERNEL_ASM}
            -o ${KERNEL_ASM_OBJ}

    DEPENDS ${KERNEL_ASM}

    COMMENT "Assembling kernel"
)


add_custom_command(
    OUTPUT ${KERNEL_BIN}

    COMMAND ${I686_LD_EXECUTABLE}
            -m elf_i386
            -T ${LINKER_SCRIPT}
            -o ${KERNEL_BIN}
            ${KERNEL_ASM_OBJ}
            ${KERNEL_CPP_OBJ}

    DEPENDS
        ${KERNEL_ASM_OBJ}
        ${KERNEL_CPP_OBJ}
        ${LINKER_SCRIPT}

    COMMENT "Linking kernel"
)

add_custom_target(kernel ALL
    DEPENDS ${KERNEL_BIN}
)


add_custom_command(
    OUTPUT ${OS_IMAGE}

    COMMAND dd
            if=/dev/zero
            of=${OS_IMAGE}
            bs=512
            count=2880
            status=none

    # Bootloader → sector 0
    COMMAND dd
            if=${BOOT_BIN}
            of=${OS_IMAGE}
            bs=512
            seek=0
            conv=notrunc
            status=none

    # Kernel → sector 1
    COMMAND dd
            if=${KERNEL_BIN}
            of=${OS_IMAGE}
            bs=512
            seek=1
            conv=notrunc
            status=none

    DEPENDS
        ${BOOT_BIN}
        ${KERNEL_BIN}

    COMMENT "Creating bootable disk image"
)

add_custom_target(image ALL
    DEPENDS ${OS_IMAGE}
)



add_custom_target(run
    COMMAND
        ${QEMU_EXECUTABLE}
        -drive format=raw,file=${OS_IMAGE}

    DEPENDS image

    USES_TERMINAL
)

add_custom_command(
    OUTPUT ${KERNEL_CPP_OBJ}
    COMMAND ${I686_GXX_EXECUTABLE}
            -m32
            -ffreestanding
            -fno-exceptions
            -fno-rtti
            -fno-stack-protector
            -fno-pic
            -fno-pie
            -c
            ${KERNEL_CPP}
            -o ${KERNEL_CPP_OBJ}
    DEPENDS
            ${KERNEL_CPP}
            ${CMAKE_SOURCE_DIR}/src/kernel.h
    COMMENT "Compiling kernel C++"
)

add_custom_command(
    OUTPUT ${KERNEL_ASM_OBJ}
    COMMAND ${NASM_EXECUTABLE}
            -f elf32
            ${KERNEL_ASM}
            -o ${KERNEL_ASM_OBJ}
    DEPENDS ${KERNEL_ASM}
    COMMENT "Assembling kernel"
)

add_custom_command(
    OUTPUT ${KERNEL_BIN}
    COMMAND ${I686_LD_EXECUTABLE}
            -m elf_i386
            -T ${LINKER_SCRIPT}
            -o ${KERNEL_BIN}
            ${KERNEL_ASM_OBJ}
            ${KERNEL_CPP_OBJ}
    DEPENDS
            ${KERNEL_ASM_OBJ}
            ${KERNEL_CPP_OBJ}
            ${LINKER_SCRIPT}
    COMMENT "Linking kernel"
)


