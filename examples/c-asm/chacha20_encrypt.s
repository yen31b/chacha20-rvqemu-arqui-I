.section .text
.global chacha20_encrypt

# En C se veria como: void chacha20_encrypt(uint8_t       *output,  // a0
#                       const uint8_t *input,   // a1
#                       uint32_t       len,     // a2
#                       const uint32_t *key,    // a3 (8 words = 32 bytes)
#                       uint32_t       counter, // a4 
#                       const uint32_t *nonce)  // a5 (3 words = 12 bytes)

chacha20_encrypt:

# Register allocation after prologue (prepare stack and save registers):
#   s0 = output pointer
#   s1 = input pointer
#   s2 = message length (bytes)
#   s3 = pointer to state buffer  
#   s4 = pointer to keystream buffer
#   s5 = bytes processed so far
#   s6 = key pointer 
#   s7 = initial counter value 
#   s8 = nonce pointer 
#   s9 = chunk size for current block (min(64, remaining))

    addi sp, sp, -176
    sw ra,  172(sp)
    sw s0,  168(sp)
    sw s1,  164(sp)
    sw s2,  160(sp)
    sw s3,  156(sp)
    sw s4,  152(sp)
    sw s5,  148(sp)
    sw s6,  144(sp)
    sw s7,  140(sp)
    sw s8,  136(sp)
    sw s9,  132(sp)

    # Move arguments to preserved registers (registers saved by called function) before the other calls
    mv s0, a0          # output
    mv s1, a1          # input
    mv s2, a2          # len
    mv s6, a3          # key pointer
    mv s7, a4          # counter
    mv s8, a5          # nonce pointer

    # addresses for two buffers on stack
    addi s3, sp, 64    # state buffer
    addi s4, sp, 0     # keystream buffer
    li   s5, 0         # bytes processed = 0

    # Build initial ChaCha20 state of RFC 8439:
    #
    #   state[ 0...3] = expand 32-byte k
    #   state[ 4...11] = key[0..7]
    #   state[12]  = counter
    #   state[13...15] = nonce[0..2]

    # state[0...3] constants ASCII expa nd 3 2-by te k:
    li t0, 0x61707865
    sw t0,  0(s3)
    li t0, 0x3320646e
    sw t0,  4(s3)
    li t0, 0x79622d32
    sw t0,  8(s3)
    li t0, 0x6b206574
    sw t0, 12(s3)

# state[4...11]: copy 8 words from keys
    li t0, 0

copy_key_loop:
    bge  t0, 8, copy_key_loop_end
    slli t1, t0, 2           # byte offset = i * 4
    add  t2, s6, t1          # &key[i]
    lw   t3, 0(t2)           # load key word
    addi t4, s3, 16          # &state[4] (base of key region)
    add  t4, t4, t1          # &state[4 + i]
    sw   t3, 0(t4)
    addi t0, t0, 1
    j    copy_key_loop
copy_key_loop_end:

    # state[12]: counter  (word offset 12 * 4 = 48)
    sw s7, 48(s3)

    # state[13...15]: copy 3 words from nonce
    lw t0, 0(s8)
    sw t0, 52(s3)            # offset 13*4 = 52
    lw t0, 4(s8)
    sw t0, 56(s3)            # offset 14*4 = 56
    lw t0, 8(s8)
    sw t0, 60(s3)            # offset 15*4 = 60

#Encryption
encrypt_loop:
    bge s5, s2, encrypt_loop_end

    # Generate keystream
    mv a0, s4        # a0 = keystream buffer
    mv a1, s3       # a1 = state (read-only inside chacha20_block)
    
    # call chacha20_block function with the keystream buffer a0 and state a1
    call chacha20_block

    # chunk_size = min(64, len - bytes_processed)
    sub t0, s2, s5    # remaining = len - bytes_processed
    li  t1, 64
    blt t0, t1, use_remaining
    li  t0, 64

use_remaining:
    mv s9, t0                # s9 = chunk_size

    # XOR input bytes with keystream bytes → output
    li t0, 0                 # byte index i = 0
xor_loop:
    bge  t0, s9, xor_loop_end
    add  t1, s1, s5          # input + bytes_processed
    add  t1, t1, t0          # &input[bytes_processed + i]
    lbu  t2, 0(t1)           # load input byte 
    add  t3, s4, t0          # &keystream[i]
    lbu  t4, 0(t3)           # load keystream byte 
    xor  t2, t2, t4          # encrypted byte
    add  t5, s0, s5          # base: output + bytes_processed
    add  t5, t5, t0          # &output[bytes_processed + i]
    sb   t2, 0(t5)           # store encrypted byte
    addi t0, t0, 1
    j    xor_loop

xor_loop_end:

    add s5, s5, s9           # bytes_processed += chunk_size

    # Increment counter for next block: state[12] at offset 48
    lw   t0, 48(s3)
    addi t0, t0, 1
    sw   t0, 48(s3)

    j encrypt_loop
encrypt_loop_end:

    # Restore the preserved registers ()
    lw ra,  172(sp)
    lw s0,  168(sp)
    lw s1,  164(sp)
    lw s2,  160(sp)
    lw s3,  156(sp)
    lw s4,  152(sp)
    lw s5,  148(sp)
    lw s6,  144(sp)
    lw s7,  140(sp)
    lw s8,  136(sp)
    lw s9,  132(sp)
    addi sp, sp, 176
    ret
