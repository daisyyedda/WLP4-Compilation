.import init
.import new
.import delete
.import print
lis $4
.word 4
lis $11
.word 1
lis $15
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
lis $17
.word 8
sub $30, $30, $17
lis $3
.word 0
sw $3, -8($29)
lis $3
.word 0
sw $3, -12($29)
lo0:
lw $3, -12($29)
add $14, $3, $0
lw $3, -4($29)
slt $3, $14, $3
beq $3, $0, eW0
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
jalr $15
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
beq $0, $0, lo0
eW0:
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, -8($29)
lis $3
.word 0
sw $3, -12($29)
lo1:
lw $3, -12($29)
add $14, $3, $0
lw $3, -4($29)
slt $3, $14, $3
beq $3, $0, eW1
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
jalr $15
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
beq $0, $0, lo1
eW1:
lis $3
.word 0
add $30, $30, $17
jr $31
FfastSort: 
sub $29, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
add $14, $3, $0
lis $3
.word 0
slt $3, $14, $3
beq $3, $0, sK2
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, eI2
sK2:
eI2:
lw $3, 4($29)
add $14, $3, $0
lis $3
.word 0
slt $3, $14, $3
beq $3, $0, sK3
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, eI3
sK3:
eI3:
lw $3, 4($29)
add $14, $3, $0
lw $3, 8($29)
slt $3, $3, $14
sub $3, $11, $3
beq $3, $0, sK4
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, eI4
sK4:
eI4:
lw $3, 0($29)
add $14, $3, $0
lis $3
.word 0
slt $6, $3, $14
slt $7, $14, $3
add $3, $6, $7
sub $3, $11, $3
beq $3, $0, sK5
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word Fpartition
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, -4($29)
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, 0($29)
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, 0($29)
beq $0, $0, eI5
sK5:
eI5:
lis $3
.word 0
add $30, $29, $4
jr $31
Fpartition: 
sub $29, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
sw $3, 0($29)
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -4($29)
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -8($29)
lo6:
lw $3, -16($29)
add $14, $3, $0
lis $3
.word 0
slt $6, $3, $14
slt $7, $14, $3
add $3, $6, $7
sub $3, $11, $3
beq $3, $0, eW6
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($29)
lo7:
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $14, $3, $0
lw $3, 0($29)
slt $3, $14, $3
beq $3, $0, eW7
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($29)
beq $0, $0, lo7
eW7:
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -8($29)
lo8:
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $14, $3, $0
lw $3, 0($29)
slt $3, $3, $14
beq $3, $0, eW8
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -8($29)
beq $0, $0, lo8
eW8:
lw $3, -4($29)
add $14, $3, $0
lw $3, -8($29)
slt $3, $14, $3
sub $3, $11, $3
beq $3, $0, sK9
lw $3, -8($29)
sw $3, -20($29)
lis $3
.word 241
sw $3, -16($29)
beq $0, $0, eI9
sK9:
eI9:
lw $3, -16($29)
add $14, $3, $0
lis $3
.word 241
slt $6, $3, $14
slt $7, $14, $3
add $3, $6, $7
beq $3, $0, sK10
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
sw $3, -12($29)
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $16, $0, $3
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $16, 0($3)
lw $3, -12($29)
add $16, $0, $3
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $16, 0($3)
beq $0, $0, eI10
sK10:
eI10:
beq $0, $0, lo6
eW6:
lw $3, -20($29)
add $30, $29, $4
jr $31
