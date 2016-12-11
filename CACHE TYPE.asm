ORG 100H

MOV SI,500H 
MOV [SI+3],2
MOV [SI+4],8
MOV [SI+5],1
MOV [SI+6],9
MOV [SI+9],2
MOV [SI+0AH],11H          ;MUL
MOV [SI+0CH],12H            ;SUB BLANK
MOV [SI+11H],3
MOV [SI+12H],13H      ;PLUS
MOV [SI+21H],4
MOV [SI+41H],5
MOV [SI+60H],6
MOV [SI+1],7


MOV CL,60H 

L1: INC SI
    DEC CL
    CMP CL,0
    JNE L1
    MOV AL,[SI] 
    MOV SI,500H
