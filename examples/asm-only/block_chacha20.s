.global chacha20_block

# En C seria como void block(uint32_t *output_block, const uint32_t *initial_state);
# a0 (output_block): Pointer to a 64-byte buffer where the keystream will be written.
# a1 (initial_state): Pointer to the initial state keys nonce, counter

chacha20_block:
    # In stack: 
    # Initial state: 16 words, 4 bytes each = 64 bytes 
    #working state: 16 words, 4 bytes each = 64 bytes  
    # 48 bytes save registers (ra, s0-s10)
    # 176 bytes
    addi sp, sp, -176
    sw ra, 172(sp)
    sw s0, 168(sp)
    sw s1, 164(sp)
    sw s2, 160(sp)
    sw s3, 156(sp)
    sw s4, 152(sp)
    sw s5, 148(sp)
    sw s6, 144(sp)
    sw s7, 140(sp)
    sw s8, 136(sp)
    sw s9, 132(sp)
    sw s10, 128(sp) 
    
    addi s0, sp, 64    # s0: pointer to initial state on stack (sp + 64)
    addi s1, sp, 0     # s1: pointer to working state on stack (sp + 0)
    mv   s2, a0        # s2:pointer to output buffer (a0)
    mv   s3, a1        # s3: pointer to initial state source (a1)

    # Copy initial state from a1 into both stack areas
    li t0, 0          # i = 0


copy_loop:
    bge t0, 16, copy_loop_end
    slli t1, t0, 2    # offset = i * 4
    add t2, s3, t1    # Source address
    lw t3, 0(t2)      # Load word

    add t4, s0, t1    # Destination address (local initial state)
    sw t3, 0(t4)
    add t5, s1, t1    # Destination address (working state)
    sw t3, 0(t5)
    
    addi t0, t0, 1    # i++
    j copy_loop
copy_loop_end:

    # 10 double rounds
    li s4, 0        # i = 0
main_loop:
    bge s4, 10, main_loop_end

    #Columnas

    # QR(0, 4, 8, 12)
    addi a0, s1, 0   # &work_state[0]
    addi a1, s1, 16  # &work_state[4]
    addi a2, s1, 32  # &work_state[8]
    addi a3, s1, 48  # &work_state[12]
    call chacha20_quarter_round

    # QR(1, 5, 9, 13)
    addi a0, s1, 4   # &work_state[1]
    addi a1, s1, 20  # &work_state[5]
    addi a2, s1, 36  # &work_state[9]
    addi a3, s1, 52  # &work_state[13]
    call chacha20_quarter_round

    # QR(2, 6, 10, 14)
    addi a0, s1, 8   # &work_state[2]
    addi a1, s1, 24  # &work_state[6]
    addi a2, s1, 40  # &work_state[10]
    addi a3, s1, 56  # &work_state[14]
    call chacha20_quarter_round

    # QR(3, 7, 11, 15)
    addi a0, s1, 12  # &work_state[3]
    addi a1, s1, 28  # &work_state[7]
    addi a2, s1, 44  # &work_state[11]
    addi a3, s1, 60  # &work_state[15]
    call chacha20_quarter_round

    # Diagonales

    # QR(0, 5, 10, 15)
    addi a0, s1, 0   # &work_state[0]
    addi a1, s1, 20  # &work_state[5]
    addi a2, s1, 40  # &work_state[10]
    addi a3, s1, 60  # &work_state[15]
    call chacha20_quarter_round

    # QR(1, 6, 11, 12)
    addi a0, s1, 4   # &work_state[1]
    addi a1, s1, 24  # &work_state[6]
    addi a2, s1, 44  # &work_state[11]
    addi a3, s1, 48  # &work_state[12]
    call chacha20_quarter_round

    # QR(2, 7, 8, 13)
    addi a0, s1, 8   # &work_state[2]
    addi a1, s1, 28  # &work_state[7]
    addi a2, s1, 32  # &work_state[8]
    addi a3, s1, 52  # &work_state[13]
    call chacha20_quarter_round

    # QR(3, 4, 9, 14)
    addi a0, s1, 12  # &work_state[3]
    addi a1, s1, 16  # &work_state[4]
    addi a2, s1, 36  # &work_state[9]
    addi a3, s1, 56  # &work_state[14]
    call chacha20_quarter_round

    addi s4, s4, 1 # i++
    j main_loop
main_loop_end:

    li t0, 0 # i = 0
sum_final_loop:
    bge t0, 16, sum_final_loop_end
    slli t1, t0, 2    # offset = i * 4
    
    add t2, s0, t1    # Address in initial state
    lw t3, 0(t2)      # Load initial word
    
    add t4, s1, t1    # Address in working state
    lw t5, 0(t4)      # Load working word
    
    add t6, t3, t5    # Word-by-word sum
    
    add t4, s2, t1    # Address in output buffer
    sw t6, 0(t4)      # Store result

    addi t0, t0, 1    # i++
    j sum_final_loop

sum_final_loop_end:
    #reset de los registros
    lw ra, 172(sp)
    lw s0, 168(sp)
    lw s1, 164(sp)
    lw s10, 128(sp)
    addi sp, sp, 176
    ret