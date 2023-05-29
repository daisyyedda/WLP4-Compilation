.import init
.import new
.import delete
.import print
lis $4
.word 4
lis $11
.word 1
lis $17
.word print
wain: 
sub $29, $30, $4
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4 
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word init
jalr $5
add $30, $30, $4
lw $31, -4($30)
lis $5
.word 16
sub $30, $30, $5
add $3, $0, $11
sw $3, -8($29)
lis $3
.word 0
sw $3, -12($29)
lis $3
.word 0
sw $3, -16($29)
add $3, $0, $11
sw $3, -20($29)
loop0:
lw $3, -12($29)
add $14, $3, $0
lis $3
.word 32
slt $3, $14, $3
beq $3, $0, endWhile0
lis $3
.word 1023
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word new
jalr $5
add $30, $30, $4
lw $31, -4($30)
bne $3, $0, 1
add $3, $11, $0
sw $3, -20($29)
lis $3
.word 0
sw $3, -16($29)
loop1:
lw $3, -16($29)
add $14, $3, $0
lis $3
.word 1023
slt $3, $14, $3
beq $3, $0, endWhile1
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -16($29)
add $30, $30, $4
lw $5, -4($30)
mult $5, $3
mflo $3
sw $3, -4($30)
sub $30, $30, $4
lw $3, -20($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -16($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
lw $3, -16($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -16($29)
beq $0, $0, loop1
endWhile1:
lw $3, -20($29)
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word delete
jalr $5
add $30, $30, $4
lw $31, -4($30)
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, loop0
endWhile0:
lw $3, -4($29)
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word new
jalr $5
add $30, $30, $4
lw $31, -4($30)
bne $3, $0, 1
add $3, $11, $0
sw $3, -8($29)
lis $3
.word 0
sw $3, -12($29)
loop2:
lw $3, -12($29)
add $14, $3, $0
lw $3, -4($29)
slt $3, $14, $3
beq $3, $0, endWhile2
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, loop2
endWhile2:
lis $3
.word 0
sw $3, -16($29)
loop3:
lw $3, -16($29)
add $14, $3, $0
lis $3
.word 1024
slt $3, $14, $3
beq $3, $0, endWhile3
lis $3
.word 1
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word new
jalr $5
add $30, $30, $4
lw $31, -4($30)
bne $3, $0, 1
add $3, $11, $0
sw $3, -20($29)
lis $3
.word 241
sw $3, -4($30)
sub $30, $30, $4
lw $3, -20($29)
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
lw $3, -20($29)
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word delete
jalr $5
add $30, $30, $4
lw $31, -4($30)
lw $3, -16($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -16($29)
beq $0, $0, loop3
endWhile3:
lis $3
.word 0
sw $3, -12($29)
loop4:
lw $3, -12($29)
add $14, $3, $0
lw $3, -4($29)
slt $3, $14, $3
beq $3, $0, endWhile4
sw $1, -4($30)
sub $30, $30, $4
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
jalr $17
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
sw $1, -4($30)
sub $30, $30, $4
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
jalr $17
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, loop4
endWhile4:
lw $3, -8($29)
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word delete
jalr $5
add $30, $30, $4
lw $31, -4($30)
lis $3
.word 0
lis $5
.word 16
add $30, $30, $5
jr $31
