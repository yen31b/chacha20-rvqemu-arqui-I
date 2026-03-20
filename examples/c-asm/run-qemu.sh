#!/bin/bash

# Run QEMU with GDB server for ChaCha20 main program
echo -e "\033[1;36m---------------------------------------------\033[0m\n" 
echo -e "\033[1;36m ChaCha20 Main Program - QEMU with GDB Server\033[0m\n"
echo -e "\033[1;36m---------------------------------------------\033[0m\n"
echo "Starting QEMU with GDB server on port 1234..."
echo "In another terminal, run: gdb-multiarch chacha20_main.elf"
echo "Then in GDB: target remote :1234"
echo ""
echo "Useful GDB commands:"
echo "  break main              - break at program entry"
echo "  break run_vector        - break at each test vector (call three times for each test vector)"
echo "  break chacha20_encrypt  - break dentro de la funcion de encriptado en ASM"
echo "  info registers         - show register values"
echo "  step / next / continue - commands to move through the code"
echo "  q                      - quit GDB"
echo ""
echo "  Inspect buffers (size for each test vector):"
echo "  x/64xb  &keystream     - TV#1 keystream  (64 bytes)"
echo "  x/375xb &keystream     - TV#2 keystream  (375 bytes)"
echo "  x/127xb &keystream     - TV#3 keystream  (127 bytes)"
echo ""
echo "  x/64xb  &ciphertext    - TV#1 ciphertext (64 bytes)"
echo "  x/375xb &ciphertext    - TV#2 ciphertext (375 bytes)"
echo "  x/127xb &ciphertext    - TV#3 ciphertext (127 bytes)"
echo ""
echo "  x/64xb  &recovered     - TV#1 recovered  (64 bytes)"
echo "  x/375xb &recovered     - TV#2 recovered  (375 bytes)"
echo "  x/127xb &recovered     - TV#3 recovered  (127 bytes)"

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel chacha20_main.elf \
    -S \
    -gdb tcp::1234