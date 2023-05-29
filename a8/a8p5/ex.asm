.import init
.import new
.import delete
.import print
lis $4
.word 4
lis $11
.word 1
beq $0, $0, wain
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
sub $30, $30, $4
add $3, $0, $11
sw $3, -8($29)
lis $3
.word 2
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
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
sw $1, -4($30)
sub $30, $30, $4
lw $3, -8($29)
lw $3, 0($3)
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
sw $1, -4($30)
sub $30, $30, $4
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
lw $3, -8($29)
beq $3, $11, deleteNULL0
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word delete
jalr $5
add $30, $30, $4
lw $31, -4($30)
deleteNULL0:
lw $3, -4($29)
add $30, $30, $4
jr $31
