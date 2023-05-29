lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
lis $12
.word 4
sub $30, $30, $12
sw $1, 0($30)
sw $2, -4($30)
lw $3, -4($29)
sw $3, 0($29)
lw $3, 0($29)
add $30, $29, $4
jr $31
