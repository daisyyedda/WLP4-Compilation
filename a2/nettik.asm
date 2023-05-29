      lis $10           ; address of eof buffer
      .word 0xFFFFFFFF
      lis $1            ; address of input buffer
      .word 0xFFFF0004
      lis $2            ; address of output buffer
      .word 0xFFFF000c
      lis $11           ; constant 1
      .word 1 
      lis $3            ; store the number of bytes in $3
      .word 0
      lis $6            ; use $6 for temp value
      .word 4     
      lis $7            ; use $7 for temp value
      .word 0

loop: lw $4, 0($1)      ; load one char from stdin into $4
      beq $4, $10, copy ; check eof
      sw $4, -4($30)    ; store the char onto stack
      sub $30, $30, $6  ; update stack pointer
      add $3, $3, $11   ; increment number of bytes
      beq $0, $0, loop  ; next iteration

copy: add $7, $3, $0    ; copy the number of bytes to $7

pop:  add $30, $30, $6  ; update stack pointer
      lw $5, -4($30)    ; load the char from stack
      sw $5, 0($2)      ; output $5 to stdout   
      sub $7, $7, $11   ; decrement number of bytes
      bne $7, $0, pop   ; next iteration

end:  jr $31            ; return