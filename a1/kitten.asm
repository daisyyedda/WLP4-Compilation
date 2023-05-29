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
loop: lw $4, 0($1)      ; load one char from stdin into $4
      beq $4, $10, end  ; check eof
      sw $4, 0($2)      ; output $4 to stdout
      add $3, $3, $11   ; increment number of bytes
      beq $0, $0, loop  ; next iteration
end:  jr $31            ; return