.import readWord
.import printHex

add $10, $0, $31    ;; $10 = $31
add $7, $0, $0      ;; $7 = 0
lis $4              ;; $4 = 4
.word 4
lis $9
.word readWord
lis $5
.word printHex
lis $12
.word 12

jalr $9             ;; skip header
jalr $9             ;; skip header
jalr $9             ;; skip header
sub $6, $3, $12     ;; $6 = $3 - 12 (number of bytes to print after skipping header)
 
print:
beq $6, $7, end     
add $7, $7, $4      ;; $7 = $7 + 4
jalr $9             ;; readWord    
add $1, $3, $0      ;; $1 = $3
jalr $5             ;; printHex 
beq $0, $0, print

end:
add $31, $10, $0    ;; $31 = $10
jr $31