beq $0, $0, wain
wain: 
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4 
sub $30, $30, $4
add $3, $0, $11
sw $3, -8($29)
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -8($29)
loop0:
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
add $30, $30, $4
lw $5, -4($30)
sltu $3, $5, $3
beq $3, $0, endWhile0
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, 0($29)
beq $0, $0, loop0
endWhile0:
lw $3, 0($29)
lw $3, 0($3)
add $30, $30, $4
jr $31
