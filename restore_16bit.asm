ORG 100H
NUM DW 0EH      ;;;;;;;NUMERATOR
DEN DW 03H      ;;;;;;;DENOMINATOR
CTR DB 10H
  
MOV SP,0300H
MOV CX,NUM         ;;;;;;;;; PARTIAL REMAINDER P

MOV AX,DEN            ;;;;DIVISOR

;MOV BX,0

MOV DL,00H ;; FOR CHECKING CARRY

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;INITIALIZATION DONE

;;;;;;;;;;;;;START OF LOOP

SOL:DEC CTR
    SHL CX,1    
    RCL BX,1
    
    PUSH BX
    
    SUB BX,AX
    
    ADC DL,0H            ;CHECKING CARRY FLAG
    
    CMP DL,00H
    
    CLC                  ;CLEARING CARRY FLAG
    MOV DL,00H           
    
    JE RESPOS

RESNEG: POP BX            ;;;;RESTORING THE VALUE IF BX-AX IS NEG
        OR CX,00H
        CMP CTR,00H
        JE EOL
        JMP SOL
      
RESPOS: OR CX,01H            ;;;; ADD AH,01H
        CMP CTR,00H
        JE EOL
        JMP SOL
  
  EOL: 
        ;ROR CH,4
        END     
    
     


;;;;;;;;;;;;;;;;END OF PROG