#lang racket

; CS241 A3 (MIPS assembler written in Racket)

(require "scanning.rkt")

(define-struct label (id value) #:transparent)

; check the range of integers

; .word i
(define (outputWord op)
  (write-byte (bitwise-and (arithmetic-shift op -24) #xff)) 
  (write-byte (bitwise-and (arithmetic-shift op -16) #xff)) 
  (write-byte (bitwise-and (arithmetic-shift op -8) #xff)) 
  (write-byte (bitwise-and op #xff)))

; add $d, $s, $t
(define (outputAddSubSltSltu op)
  (define d (token-int-value (first op)))
  (define s (token-int-value (third op)))
  (define t (token-int-value (last op)))

  (define instr (bitwise-ior (arithmetic-shift 0 26)
                             (arithmetic-shift s 21)
                             (arithmetic-shift t 16)
                             (arithmetic-shift d 11)
                             32))  ; encoding for 
  (outputWord instr)                           
)

; beq $s, $t, i
(define (outputBeqBne op)
  (define s (token-int-value (first op)))
  (define t (token-int-value (third op)))
  (define i (token-int-value (last op)))
  (define instr (bitwise-ior (arithmetic-shift 4 26)
                             (arithmetic-shift s 21)
                             (arithmetic-shift t 16)
                             (bitwise-and i #xFFFF))) 
  (outputWord instr)
)

; (isValidLabel id)
(define (isValidLabel lab)
)

(for [(line (in-lines))]
  (define instr (scan line))
  (cond ; comment
        [(empty? instr) (void)]
        ; label
        [(symbol=? (token-kind (first instr)) 'LABEL)]
        ; .word
        [(symbol=? (token-kind (first instr)) 'WORD)
          (cond ; .word label
                [(and (symbol? (token-kind (second instr)) 'ID)
                      (empty? (third instr)))
                 (outputWord .....)]
                ; .word num
                [(and (or (symbol? (token-kind (second instr)) 'INT)
                          (symbol? (token-kind (second instr)) 'REG)
                          (symbol? (token-kind (second instr)) 'HEXINT))
                      (empty? (third instr)))
                 (outputWord (token-int-value (first (rest instr))))]
          )
        ]
        ; add $d, $s, $t
        [(and (symbol=? (token-kind (first instr)) 'ID)
              (string=? (token-lexeme (first instr)) "add"))
         (outputAdd (rest instr))]
        ; sub $d, $s, $t
        
        ; slt $d, $s, $t
        ; sltu $d, $s, $t

        ; mult $s, $t
        ; multu $s, $t
        ; div $s, $t
        ; divu $s, $t

        ; mfhi $d
        ; mflo $d
        ; lis $d
      

        ; lw $t, i($s)
        ; sw $t, i($s)

        ; beq $s, $t, i
         [(and (symbol=? (token-kind (first instr)) 'ID)
               (string=? (token-lexeme (first instr)) "beq"))
         (outputBeq (rest instr))] 
        ; bne $s, $t, i

        ; jr $s
        ; jalr $s

        ; report to error stream
        [else (raise-user-error 'ERROR "Invalid instruction.")]
  ) 
)

; define opcode
(define (opcode instr))

; output the binary encoding of all instructions

(define (outputInstr labelLi instrLi)
  )
; check valid label
(define (isLabel instr))

; check valid instruction
(define (isInstr instr))

; store instructions and labels
(define (scanFile len labelLi instrLi) 
  (define instr (scan line))
  (for [(line (in-lines))]
    (cond [(empty? instr) (void)] ; comment
          [(and (not (isLabel instr)) (isInstr instr))
           (cons instr instrLi)] ; instruction
          [(isLabel instr)
          (add label to label list)] ; label
          [else (error 'ERROR)] ; invalid instruction
    )
  )
  (outputInstr labelLi (reverse instrLi))
)

; ultimate driver function
(scanFile 0 empty empty)