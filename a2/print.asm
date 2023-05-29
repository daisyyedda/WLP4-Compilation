print:
; $1 stores the value as two's complement integer (preserved)

; push the preserved values to stack
sw $1, -4($30)  
sw $2, -8($30) 
sw $3, -12($30) 
sw $4, -16($30) 
sw $5, -20($30) 
sw $6, -24($30)   
sw $7, -28($30)
lis $3
.word 28
sub $30, $30, $3  ; update stack pointer 

lis $2            ; address of output buffer
.word 0xFFFF000c
add $6, $0, $0    ; initialize counter to 0
lis $7            ; constant 1
.word 1

slt $3, $1, $0    ; check if the number is positive
beq $3, $0, loop  ; positive case
lis $3            ; negative case, print negative sign
.word 45
sw $3, 0($2)
sub $1, $0, $1    ; negate the number

loop:
lis $4        
.word 10
divu $1, $4       ; divide the number by 10
mflo $1           ; store quotient in $1
mfhi $5           ; store remainder in $5
lis $4        
.word 48
add $5, $5, $4    ; convert remainder to ascii
sw $5, -4($30)
lis $4        
.word 4
sub $30, $30, $4  ; decrease stack pointer
add $6, $6, $7    ; increment the counter
bne $1, $0, loop    

loop2:
lis $4        
.word 4
add $30, $30, $4  ; increase stack pointer
lw $1, -4($30)
sw $1, 0($2)      ; print digit
sub $6, $6, $7    ; decrement the counter
bne $6, $0, loop2 ; keep printing if counter is not zero

end:
; print the newline character
lis $3
.word 10
sw $3, 0($2)
; pop all registers we pushed
lis $3
.word 28
add $30, $30, $3  ; update stack pointer   
lw $1, -4($30)  
lw $2, -8($30) 
lw $3, -12($30) 
lw $4, -16($30) 
lw $5, -20($30) 
lw $6, -24($30)   
lw $7, -28($30)
jr $31