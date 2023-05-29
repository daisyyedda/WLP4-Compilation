;; offset 0
;; table size 1
;; offset -4
;; table size 2
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
lis $12
.word 8
sub $30, $30, $12
sw $1, 0($29)
sw $2, -4($29)
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
div $5, $3
mflo $3
add $30, $29, $4
jr $31
