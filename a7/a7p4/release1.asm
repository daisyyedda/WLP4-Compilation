;; offset 0
;; table size 1
;; offset -4
;; table size 2
;; offset -8
;; table size 3
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
lis $12
.word 12
sub $30, $30, $12
sw $1, 0($29)
sw $2, -4($29)
lis $3
.word 0
sw $3, -8($29)
loop0:
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $4, $30
lw $5, -4($30)
slt $6, $3, $5
slt $7, $5, $3
add $3, $6, $7
beq $3, $0, endWhile0
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -8($29)
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 2
add $30, $30, $4
lw $5, -4($30)
div $5, $3
mfhi $3
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
add $30, $4, $30
lw $5, -4($30)
slt $6, $3, $5
slt $7, $5, $3
add $3, $6, $7
sub $3, $3, $11
beq $3, $0, else1
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 2
add $30, $30, $4
lw $5, -4($30)
div $5, $3
mflo $3
sw $3, 0($29)
beq $0, $0, endif1
else1:
lis $3
.word 3
sw $3, -4($30)
sub $30, $30, $4
lw $3, 0($29)
add $30, $30, $4
lw $5, -4($30)
mult $5, $3
mflo $3
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, 0($29)
endif1:
beq $0, $0, loop0
endWhile0:
lw $3, -8($29)
add $30, $29, $4
jr $31
