ORG 100H
MOV DI,10H
MOV CX,09H    ;;[P]  NUM
MOV AX,03H              ;DEN
SOL: DEC DI
     SHL CX,1
     RCL BX,1
     CMP BX,8000H
     JAE PNEG                                              
PPOS:SUB BX,AX
     OR CX,01H
     JMP CND
PNEG:ADD BX,AX 
     SUB CX,01
 CND:CMP DI,00H
     JNZ SOL
     END                                                    