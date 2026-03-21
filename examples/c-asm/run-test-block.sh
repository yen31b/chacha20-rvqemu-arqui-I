#!/bin/bash

# Run chacha20_block tests on QEMU
echo "Running chacha20_block tests..."
echo "Press Ctrl+C to exit QEMU"
echo ""

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel test_block_chacha20.elf
