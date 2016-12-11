ORG 100H
MOV BP,400H
MOV AX,7H
MOV [BP],AX
MOV AX,04H
MOV [BP+1],AX
MOV AX,05H
MOV [BP+2],AX
MOV AX,03H
MOV [BP+3],AX
MOV AX,06H
MOV [BP+3],AX

MOV AX,0
PUSH AX
MOV AX,4H
PUSH AX
CALL QUIS

QUIS PROC NEAR
     POP DI
     POP DI
     POP SI
     MOV AX,SI
     MOV BX,DI
     MOV CL,[DI+BP]
 SHW1:CMP SI,DI
       JA EW1
       SHW2: CMP [BP+SI],CL
              JAE SHW3
              INC SI
              JMP SHW2
              SHW3: CMP [BP+DI],CL
                    JBE EW3
                    DEC DI
                    JMP SHW3
          EW3: CMP SI,DI
          JA EOIF
          PUSH CX
          MOV CH,[BP+SI]
          MOV CL,[BP+DI]
          MOV [BP+SI],CL
          MOV [BP+DI],CH
          POP CX
          INC SI
          CMP DI,0
          CMP SI,5
          JAE RECUR2
          CMP DI,5
          JAE RECUR2
          JNE EOIF
          DEC DI
          EOIF: JMP SHW1
          EW1: CMP AX,DI
          JAE RECUR1
          PUSH SI
          PUSH BX
          PUSH AX
          PUSH DI
          CALL QUIS
          RECUR1: CMP SI,BX
          JAE RECUR2
          POP BX
          POP SI
          PUSH SI
          PUSH BX
          CALL QUIS
          RECUR2: QUIS ENDP
END
    
    