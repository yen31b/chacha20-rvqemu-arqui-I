#!/bin/bash

# Build script for all ChaCha20 tests
# Requires build.sh to have been run first (needs startup.o, chacha20_quarter_round.o, block_chacha20.o)
echo "Building all ChaCha20 tests..."

# --- Test: chacha20_quarter_round ---

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
    echo "test_quarter_round.c compilation failed"
    exit 1
fi

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

if [ $? -ne 0 ]; then
    echo "Linking test_quarter_round.elf failed"
    exit 1
fi
echo "Built: test_quarter_round.elf"

# --- Test: chacha20_block ---

riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    test_block_chacha20.c \
    -o test_block_chacha20.o

if [ $? -ne 0 ]; then
    echo "test_block_chacha20.c compilation failed"
    exit 1
fi

riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    startup.o \
    test_block_chacha20.o \
    block_chacha20.o \
    chacha20_quarter_round.o \
    -T linker.ld \
    -o test_block_chacha20.elf

if [ $? -ne 0 ]; then
    echo "Linking test_block_chacha20.elf failed"
    exit 1
fi
echo "Built: test_block_chacha20.elf"

echo "All tests built successfully"
