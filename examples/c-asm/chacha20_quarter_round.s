.section .text
.global chacha20_quarter_round

# void chacha20_quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);
# use uint32_t for 32-bit register size
# Arguments:
# a0: pointer to 'a' (uint32_t *)
# a1: pointer to 'b' (uint32_t *)
# a2: pointer to 'c' (uint32_t *)
# a3: pointer to 'd' (uint32_t *)

chacha20_quarter_round:
    addi sp, sp, -16 # 16 bytes for s0, s1, s2, s3
    # save registers that call saved
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s2, 4(sp)
    sw s3, 0(sp)

    # load the values from the memory addresses
    # s0 = a, s1 = b, s2 = c, s3 = d
    lw s0, 0(a0)
    lw s1, 0(a1)
    lw s2, 0(a2)
    lw s3, 0(a3)

    # quarter round operations

    # a += b; d ^= a; d <<<= 16;
    add s0, s0, s1
    xor s3, s3, s0
    # rotation left 16 bits for d (s3)
    slli t0, s3, 16
    srli t1, s3, 16
    or   s3, t0, t1

    # c += d; b ^= c; b <<<= 12;
    add s2, s2, s3
    xor s1, s1, s2
    # rotation left 12 bits for b (s1)
    slli t0, s1, 12
    srli t1, s1, 20
    or   s1, t0, t1

    #a += b; d ^= a; d <<<= 8;
    add s0, s0, s1
    xor s3, s3, s0
    # rotation left 8 bits for d (s3)
    slli t0, s3, 8
    srli t1, s3, 24
    or   s3, t0, t1

    #c += d; b ^= c; b <<<= 7;
    add s2, s2, s3
    xor s1, s1, s2
    # rotation left 7 bits for b (s1)
    slli t0, s1, 7
    srli t1, s1, 25
    or   s1, t0, t1

    # store the results in memory
    sw s0, 0(a0)
    sw s1, 0(a1)
    sw s2, 0(a2)
    sw s3, 0(a3)

    # restore registers that call saved and return
    lw s3, 0(sp)
    lw s2, 4(sp)
    lw s1, 8(sp)
    lw s0, 12(sp)
    addi sp, sp, 16
    ret