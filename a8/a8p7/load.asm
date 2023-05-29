.import readWord
.import printHex

add $15, $0, $31    ;; $15 = $31

add $7, $0, $0      ;; $7 = 0
add $11, $0, $0     ;; $11 = 0
lis $4              ;; $4 = 4
.word 4
lis $9
.word readWord
lis $5
.word printHex
lis $12
.word 12

jalr $9             ;; read alpha
add $10, $3, $0     ;; $10 = alpha
jalr $9             ;; skip header
jalr $9             ;; skip header
sub $6, $3, $12     ;; $6 = $3 - 12
jalr $9             ;; skip header
sub $18, $3, $12    ;; $18 = $3 - 12

print1:
beq $18, $7, end1 
add $7, $7, $4      ;; $7 = $7 + 4
jalr $9             ;; readWord    
add $1, $3, $0      ;; $1 = $3
add $14, $10, $11   ;; $14 = $10 + $11  
add $11, $11, $4    ;; $11 = $11 + 4
sw $1, 0($14)       ;; store word in memory
jalr $5             ;; printHex 
beq $0, $0, print1

end1:
lis $11
.word 1
lis $12
.word 12
sub $12, $10, $12   ;; $12 = $10 - 12 = relocation offset
lis $16
.word 0x05          ;; $16 = 0x05 = ESD
lis $17
.word 0x11          ;; $17 = 0x11 = ESR

relocation:
beq $6, $7, ending  ;; relocation ends, time to print
jalr $9             ;; readWord
add $7, $7, $4      ;; $7 = $7 + 4
beq $3, $16, esd    ;; if $3 == 0x05, then ESD
bne $3, $17, noEsr  ;; if $3 == 0x11, then not ending
lis $1
.word 0x000EBB0B
jalr $5             ;; printHex
beq $0, $0, end2  
noEsr:
bne $3, $11, ending
jalr $9             ;; readWord
add $7, $7, $4      ;; $7 = $7 + 4
add $20, $3, $12
lw $21, 0($20)
add $21, $21, $12
sw $21, 0($20)
beq $0, $0, relocation

esd:
beq $6, $7, ending  ;; relocation ends, time to print
jalr $9             ;; readWord
add $7, $7, $4      ;; $7 = $7 + 4
jalr $9             ;; readWord
add $7, $7, $4      ;; $7 = $7 + 4
add $22, $3, $0
add $23, $0, $0

endingPre:
beq $6, $7, ending  ;; relocation ends, time to print
beq $22, $23, preEnd
jalr $9             ;; readWord
add $7, $7, $4      ;; $7 = $7 + 4
add $23, $23, $11   ;; $23 = $23 + 1
beq $0, $0, endingPre
preEnd:
beq $0, $0, relocation

ending:
sw $1, -4($30)
sw $3, -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $8, -28($30)
sw $9, -32($30)
sw $11, -40($30)
sw $12, -44($30)
sw $13, -48($30)
sw $14, -52($30)
sw $15, -56($30)
sw $16, -60($30)
sw $17, -64($30)
sw $20, -68($30)
sw $21, -72($30)
sw $22, -76($30)
sw $23, -80($30)
sw $30, -84($30)
sw $10, -88($30)
lis $8
.word 88
sub $30, $30, $8
sub $29, $30, $4
sw $30, 0($29)
sub $30, $30, $4
jalr $10            ;; jump to alpha
add $30, $30, $4
lw $30, 0($29)
lis $8
.word 88
add $30, $30, $8
lw $1,  -4($30)
lw $3, -12($30)
lw $4, -12($30)
lw $5, -16($30)
lw $6, -20($30)
lw $7, -24($30)
lw $8, -32($30)
lw $9, -36($30)
lw $11, -40($30)
lw $12, -44($30)
lw $13, -48($30)
lw $14, -52($30)
lw $15, -56($30)
lw $16, -60($30)
lw $17, -64($30)
lw $20, -68($30)
lw $21, -72($30)
lw $22, -76($30)
lw $23, -80($30)
lw $30, -84($30)
lw $10, -88($30)

add $7, $0, $0      ;; $7 = 0

print2:
beq $18, $7, end2
add $14, $10, $7    ;; $13 = $10 + $7
lw $1, 0($14)       ;; $1 = memory[$13]
jalr $5             ;; printHex
add $7, $7, $4      ;; $7 = $7 + 4
beq $0, $0, print2

end2:
add $31, $15, $0    ;; $31 = $15

jr $31