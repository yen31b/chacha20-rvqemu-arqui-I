#!/bin/bash

# Run QEMU with GDB server for ChaCha20 main program (Fase 5)
echo "Starting QEMU with GDB server on port 1234..."
echo "In another terminal, run: gdb-multiarch chacha20_main.elf"
echo "Then in GDB: target remote :1234"
echo ""
echo "Useful GDB commands:"
echo "  break main             - break at program entry"
echo "  break show_keystream   - break before keystream generation"
echo "  break show_encrypt     - break before encryption"
echo "  break show_decrypt     - break before decryption"
echo "  x/16xb &keystream      - view keystream bytes in memory"
echo "  x/46xb &ciphertext     - view ciphertext bytes in memory"
echo "  info registers         - show register values"
echo "  step / next / continue - navigate code"

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel chacha20_main.elf \
    -S \
    -gdb tcp::1234