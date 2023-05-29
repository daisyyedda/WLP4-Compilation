stirling:
; $1 first parameter (preserved)
; $2 second parameter (preserved) 
; $3 result (not preserved)

; $4, $5, and $6 are used as temporary registers

; push all registers we change to stack
sw $1, -4($30)    
sw $2, -8($30)
sw $31, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)
lis $4
.word 24
sub $30, $30, $4 

sltu $5, $1, $2     ; check if n < k
lis $6
.word 1
beq $5, $6, zero    
beq $1, $0, one     ; f(0, 0) 
beq $2, $0, zero    ; f(i, 0)  
beq $0, $0, recur

one:
lis $3
.word 1
beq $0, $0, end

zero:
add $3, $0, $0 
beq $0, $0, end

; if we get here we need a recursive call
recur:
lis $4
.word 1
sub $1, $1, $4      ; n-1
lis $5
.word stirling   
jalr $5             ; result of call will be in $3
mult $3, $1
mflo $6
sub $2, $2, $4      ; k-1
jalr $5             ; result of call will be in $3

add $3, $3, $6      ; (n-1) * f(n-1, k) + f(n-1, k-1)

end:
; pop all registers we pushed
lis $4
.word 24
add $30, $30, $4 
lw $1, -4($30)    
lw $2, -8($30)
lw $31, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $6, -24($30)
jr $31  