        beq $2, $1, true  ; equal case     
        beq $2, $0, true  ; is $2 zero?
        beq $1, $0, false ; is $1 zero?  
        slt $5, $2, $1    ; is $2 less than $1?
        beq $5, $1, false ; not divisible case
        divu $2, $1       ; divide $2 by $1
        mfhi $6           ; store remainder in $6         
        beq $6, $0, true  ; divisible case
        bne $6, $0, false ; not divisible case
true:   lis $3            ; divisible, place 1 in $3
        .word 1
        beq $0, $0, end   ; branch to return
false:  add $3, $0, $0    ; not divisble, place 0 in $3
end:    jr $31