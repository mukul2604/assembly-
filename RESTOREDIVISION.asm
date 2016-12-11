ORG 100H
NUM DB 06H      ;;;;;;;NUMERATOR
DEN DB 03H      ;;;;;;;DENOMINATOR
CTR DB 04H


MOV CH,NUM         ;;;;;;;;; PARTIAL REMAINDER P

MOV CL,DEN          
SHL CL,4         ;;;;;;;;; TREATED AS D

MOV AH,00H       ;;;;;;;;; QUOTIENT WIILL BE STORED HERE

MOV AL,00H ;; FOR CHECKING CARRY

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;INITIALIZATION DONE

;;;;;;;;;;;;;START OF LOOP

SOL:DEC CTR
    SHL CH,1    
    SHL AH,1
    
    PUSH CX
    
    SUB CH,CL
    
    ADC AL,0H            ;CHECKING CARRY FLAG
    
    CMP AL,00H
    
    CLC                  ;CLEARING CARRY FLAG
    MOV AL,00H           
    
    JE RESPOS

RESNEG: POP CX            ;;;;RESTORING THE VALUE IF CH-CL IS NEG
        OR AH,00H
        CMP CTR,00H
        JE EOL
        JMP SOL
      
RESPOS: OR AH,01H            ;;;; ADD AH,01H
        CMP CTR,00H
        JE EOL
        JMP SOL
  
  EOL: 
        ROR CH,4
        END     
    
     


;;;;;;;;;;;;;;;;END OF PROG