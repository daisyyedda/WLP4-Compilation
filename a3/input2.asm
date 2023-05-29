; $1 holds the starting address of the array
; $2 holds the size of the array
; $3 stores the height of the tree

; define some constants
add $4, $0, $1          ; starting address of the array

height:
; push the general-purpose registers to stack
sw $4, -4($30)
sw $5, -8($30)
sw $6, -12($30)
sw $7, -16($30)
sw $8, -20($30)
sw $9, -24($30)
sw $10, -28($30)
sw $11, -32($30)
sw $12, -36($30)
sw $13, -40($30)
sw $14, -44($30)
sw $15, -48($30)
sw $16, -52($30)
sw $31, -56($30)
lis $14
.word 56
sub $30, $30, $14

lis $5                  ; size of each memory block
.word 4  
lis $6                  ; constant 1
.word 1 
lis $7                  ; empty child indicator
.word -1

add $8, $0, $0          ; initialize the height to 0

add $9, $5, $4          ; left child (offset into array)
add $10, $9, $5         ; right child (offset into array)
lw $11, 0($9)           ; root of left subtree
lw $12, 0($10)          ; root of right subtree

bne $11, $7, recur      ; non-empty left subtree
bne $12, $7, recur      ; non-empty right subtree
add $3, $0, $6          ; add the root node to height
beq $0, $0, end         ; end the program

recur:
; load recursion address
lis $13
.word height

rightSub:
bne $11, $7, leftSub    ; check left subtree
mult $12, $5
mflo $16                ; offset into array
add $4, $1, $16         ; starting address of right subtree
jalr $13                ; recurse on right subtree
beq $0, $0, maxHeight

leftSub:
bne $12, $7, rightSub2  ; check right subtree
mult $11, $5
mflo $16                ; offset into array
add $4, $1, $16         ; starting address of left subtree
jalr $13                ; recurse on left subtree
beq $0, $0, maxHeight

rightSub2:
mult $11, $5
mflo $16                ; offset into array
add $4, $1, $16         ; starting address of left subtree
jalr $13                ; recurse on left subtree

add $8, $3, $0          ; move the height to $8

leftSub2:
mult $12, $5
mflo $16                ; offset into array
add $4, $1, $16         ; starting address of right subtree
jalr $13                ; recurse on right subtree

maxHeight:
slt $15, $3, $8
beq $15, $0, 1
add $3, $8, $0          ; update $3 to maximum height
add $3, $3, $6          ; add the root node to height
beq $0, $0, end

end:
; pop the registers pushed off the stack
lis $14
.word 56
add $30, $30, $14
lw $4, -4($30)
lw $5, -8($30)
lw $6, -12($30)
lw $7, -16($30)
lw $8, -20($30)
lw $9, -24($30)
lw $10, -28($30)
lw $11, -32($30)
lw $12, -36($30)
lw $13, -40($30)
lw $14, -44($30)
lw $15, -48($30)
lw $16, -52($30)
lw $31, -56($30)
jr $31