ORG 100H
MOV DI,10H
MOV BX,0  
MOV CX,0EH         ;;;;;;;;;BX,CX: PARTIAL REMAINDER P
MOV AX,03H            ;;;;DIVISOR
SOL:DEC DI
    SHL CX,1    
    RCL BX,1
    SUB BX,AX
    JC RESNEG
        OR CX,01H            ;;;; ADD AH,01H
        JMP CNDN    
RESNEG: ADD BX,AX             ;;;;RESTORING THE VALUE IF BX-AX IS NEG
   CNDN:CMP DI,00H
        JE EOL
        JMP SOL
      EOL:HLT    
    
     


