#!/bin/bash

# Run chacha20_encrypt tests on QEMU
echo "Running chacha20_encrypt tests..."
echo "Press Ctrl+C to exit QEMU"
echo ""

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel test_encrypt_chacha20.elf
