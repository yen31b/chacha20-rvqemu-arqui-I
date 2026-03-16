#!/bin/bash

# Build script for chacha20_quarter_round tests
echo "Building quarter round tests..."

# Compile startup assembly
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
    echo "ChaCha20 quarter round assembly compilation failed"
    exit 1
fi

# Compile test C source
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    test_quarter_round.c \
    -o test_quarter_round.o

if [ $? -ne 0 ]; then
    echo "Test C compilation failed"
    exit 1
fi

# Link object files
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    startup.o \
    test_quarter_round.o \
    chacha20_quarter_round.o \
    -T linker.ld \
    -o test_quarter_round.elf

if [ $? -eq 0 ]; then
    echo "Build successful: test_quarter_round.elf created"
else
    echo "Linking failed"
    exit 1
fi
