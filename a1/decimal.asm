          lis $2                ; address of eof buffer
          .word 0xFFFFFFFF
          lis $3                ; address of input buffer
          .word 0xFFFF0004
          lis $4                ; address of output buffer
          .word 0xFFFF000c
          lis $5                ; constant 1
          .word 1 
          lis $6                ; negative sign
          .word 45         
          lis $7                ; constant 10
          .word 10
          lis $8                ; decimal value
          .word 0
          lis $19               ; ASCII code for digit 0
          .word 48  

; readPos => read in positive integers
readPos:  lw $1, 0($3)          ; load one char from stdin into $1
          beq $1, $6, readNeg   ; negative number 
          beq $1, $2, calcPos   ; eof, time to convert decimal to binary   
          sub $1, $1, $19       ; convert char to digit
          add $21, $8, $0
          multu $8, $7          ; $8 * 10
          mflo $9               ; $9 = $8 * 10
          add $8, $1, $9        ; $8 = $8 * 10 + $1
          beq $0, $0, readPos   ; read next char     

calcPos:  sw $21, 0($4)
          beq $0, $0, end      

; readNeg => read in negative integers
readNeg:  lw $1, 0($3)          ; load one char from stdin into $1
          beq $1, $2, calcNeg   ; eof, time to convert decimal to binary   
          sub $1, $1, $19       ; convert char to digit
          add $21, $8, $0
          multu $8, $7          ; $8 * 10
          mflo $9               ; $9 = $8 * 10
          add $8, $1, $9        ; $8 = $8 * 10 + $1
          beq $0, $0, readNeg   ; read next char 

calcNeg:  sub $21, $0, $21
          sw $21, 0($4)
          beq $0, $0, end

end:      jr $31                ; return   