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
jalr $9             ;; skip header
sub $6, $3, $12     ;; $6 = $3 - 12 (number of bytes to print after skipping header)
 
print1:
beq $6, $7, end1     
add $7, $7, $4      ;; $7 = $7 + 4
jalr $9             ;; readWord    
add $1, $3, $0      ;; $1 = $3
add $14, $10, $11   ;; $12 = $10 + $11  
add $11, $11, $4    ;; $11 = $11 + 4
sw $1, 0($14)       ;; store word in memory
jalr $5             ;; printHex 
beq $0, $0, print1

end1:
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
lis $8
.word 56
sub $30, $30, $8
jalr $10            ;; jump to alpha
lis $8
.word 56
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
add $7, $0, $0      ;; $7 = 0

print2:
beq $11, $7, end2
add $13, $10, $7    ;; $13 = $10 + $7
lw $1, 0($13)       ;; $1 = memory[$13]
jalr $5             ;; printHex
add $7, $7, $4      ;; $7 = $7 + 4
beq $0, $0, print2

end2:
add $31, $15, $0    ;; $31 = $15
jr $31