#!/bin/bash

# Build script for C+assembly example
echo "Building C+assembly example..."

# Compile C source to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    example.c \
    -o example.o

if [ $? -ne 0 ]; then
    echo "C compilation failed"
    exit 1
fi

# Compile startup assembly to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    startup.s \
    -o startup.o

if [ $? -ne 0 ]; then
    echo "Startup assembly compilation failed"
    exit 1
fi

# Compile math assembly source to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    math_asm.s \
    -o math_asm.o

if [ $? -ne 0 ]; then
    echo "Math assembly compilation failed"
    exit 1
fi

# Compile ChaCha20 quarter round assembly
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    chacha20_quarter_round.s \
    -o chacha20_quarter_round.o

if [ $? -ne 0 ]; then
    echo "ChaCha20 quarter round compilation failed"
    exit 1
fi

# Compile ChaCha20 block assembly
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    block_chacha20.s \
    -o block_chacha20.o

if [ $? -ne 0 ]; then
    echo "ChaCha20 block compilation failed"
    exit 1
fi

# Compile ChaCha20 encrypt assembly
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    chacha20_encrypt.s \
    -o chacha20_encrypt.o

if [ $? -ne 0 ]; then
    echo "ChaCha20 encrypt compilation failed"
    exit 1
fi

# Compile main C program
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    chacha20_main.c \
    -o chacha20_main.o

if [ $? -ne 0 ]; then
    echo "chacha20_main.c compilation failed"
    exit 1
fi

# Link object files together
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    startup.o \
    example.o \
    math_asm.o \
    chacha20_quarter_round.o \
    block_chacha20.o \
    chacha20_encrypt.o \
    chacha20_main.o \
    -T linker.ld \
    -o example.elf

if [ $? -eq 0 ]; then
    echo "Build successful: example.elf created"
    echo "Object files: example.o, math_asm.o, chacha20_quarter_round.o, block_chacha20.o, chacha20_encrypt.o"
else
    echo "Linking failed"
    exit 1
fi