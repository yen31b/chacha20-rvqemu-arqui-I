#!/bin/bash

# Run quarter round tests on QEMU
# Output goes directly to the terminal via UART (no GDB needed)
echo "Running chacha20_quarter_round tests..."
echo "Press Ctrl+A then X to exit QEMU"
echo ""

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel test_quarter_round.elf
