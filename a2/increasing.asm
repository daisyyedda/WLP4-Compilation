          lis $13             ; size of each array element
          .word 4
          lis $16             ; constant 1
          .word 1
          
; store the last index of the array in $17
          sub $17, $2, $16

; store the address of first element in $18
          add $18, $0, $1
      
loop:     lw $1, 0($18)       ; store first array element in $1  
          add $18, $18, $13   
          lw $2, 0($18)       ; store second array element in $2
          sw $31, -4($30)     ; store the return value  
          sub $30, $30, $13  
          lis $6   
          .word compare       ; store address of compare in $6
          jalr $6             ; execute compare procedure
          add $30, $30, $13
          lw $31, -4($30)     ; load the return value  
          sub $17, $17, $16   ; decrement last index    
          beq $3, $0, end     ; not strictly increasing, return false
          beq $17, $0, end    ; reach end of the array
          beq $0, $0, loop    ; next iteration

end:      jr $31              ; end of the program