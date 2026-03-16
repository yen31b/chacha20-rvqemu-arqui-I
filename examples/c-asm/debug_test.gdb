# Connect to QEMU GDB server
target remote :1234

# Breakpoints
break _start
break main
break sum_to_n
break chacha20_quarter_round
break chacha20_block

# Show assembly and register views
layout asm
layout regs
continue