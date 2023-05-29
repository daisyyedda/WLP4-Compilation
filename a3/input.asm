.word -2147483648
.word 2147483647
.word 2147483648
.word 4294967295
.word 0xffffffff
.word 0x80000000
.word 0x7fffffff
.word -241
lis $18
.word 38
lis $10
.word label2
jalr $10
lis $17
.word 1
add $3, $4, $5
beq $3, $3, label2
beq $8, $9, 3
jalr $9
label:
  beq $0, $0, -4
  sub $18, $18, $17
  bne $18, $0, label
; this is a comment
label2:
  div $3, $4
  multu $12, $13
  mflo $14
  mfhi $15
  lw $5, -3($8)
  sw $28, 8($7)
; this is another comment
jr $31
