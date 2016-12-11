;; DIVISION BY NON RESTORATION METHOD
; INCLUDE 'EMU8086.INC'
ORG 100H
NUM DB 0EH
DEN DB 03H
CTR DB 04H


MOV CH,NUM    ;;[P] 
MOV CL,DEN
SHL CL,4     ;;;;[D], DIVISOR
MOV AH,00H     ;; QUOTIENT WILL BE STORED HERE
                                              

SOL: DEC CTR
     SHL CH,1
     SHL AH,1
     PUSH CX
     
     AND CH,80H
     CMP CH,80H
     JE PNEG                                              
        
    PPOS: POP CX
          SUB CH,CL
          ;JO LABEL
          ADD AH,01H
          CMP CTR,00H
          JNZ SOL
          JMP EOL
          
    PNEG: POP CX
          ADD CH,CL 
          ;JO LABEL
          SUB AH,01H
          CMP CTR,00H
          JNZ SOL
          JMP EOL                                              
          ;LABEL: PRINT 'OVERFLOW'
          
 EOL:   ROL CH,4
        END                                                    