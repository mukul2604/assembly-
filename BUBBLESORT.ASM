ORG 0200H
MOV BX,5
DEC BX 
MOV SI,300H
MOV AL,8
MOV [SI],AL
MOV AL,3
MOV [SI+1],AL
MOV AL,4
MOV [SI+2],AL
MOV AL,2
MOV [SI+3],AL
MOV AL,7
MOV [SI+4],AL

NXTPASS:MOV CX,BX
       

NXTCOMP:MOV AL,[SI]
       INC SI
       CMP AL,[SI]
       JBE NEXT
       XCHG AL,[SI]
       DEC SI
       MOV [SI],AL
       INC SI
NEXT:  DEC CX
       CMP CX,0
       
       JNE NXTCOMP
       SUB SI,4
       DEC BX
       
       JNZ NXTPASS

END