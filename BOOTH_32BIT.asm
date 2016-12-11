ORG 100H
MOV SI,300H
MOV SP,500H

MOV [SI],02H
MOV [SI+2],06H

MOV AX,00H
MOV BX,[SI+2]
MOV CX,BX
MOV DI,0FH

PUSH CX

AND CX,01
CMP CX,01
JNE L1
SUB AX,[SI]
SAR AX,1
RCR BX,1
L1: SAR AX,1
    RCR BX,1
  
  
  SOL: DEC DI
       POP CX
       MOV DX,CX
       SHR CX,1
       PUSH CX
       AND DX,03
      
          CMP DX,03H
          JNE TEST2
   TEST1: SAR AX,1
          RCR BX,1
          JMP EOL 
   TEST2: CMP DX,02
          JNE TEST3
          SUB AX,[SI]
          SAR AX,1
          RCR BX,1
          JMP EOL    
   TEST3: CMP DX,01
          JNE TEST1
          ADD AX,[SI]
          SAR AX,1
          RCR BX,1
          JMP EOL
 EOL: CMP DI,0
     JNZ SOL
     
    ;;;;;;;;;;;;;;;;;;;;; MOV CX,AX
;;;;;;;;;;;;;;;;;;;;     ;AND CX,8000H
;;;;;;;;;;;;;;;;;;;;     ;CMP CX,8000H
;;;;;;;;;;;;;;;;;;;;     ;JNE L2
;;;;;;;;;;;;;;;;;;;;     ;NEG BX
;;;;;;;;;;;;;;;;;;;;    ; NOT AX
     L2:     
     
     END
   
     
   
          
      