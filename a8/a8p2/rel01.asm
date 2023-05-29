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
;; factor STAR factor reached
;; factor AMP lvalue reached
;; tmp->rule is lvalue STAR factor
;; lvalue STAR factor reached
;; factor AMP lvalue reached
;; tmp->rule is lvalue ID
lis $3
.word -4
add $3, $3, $29
lw $3, 0($3)
jr $31
