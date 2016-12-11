ORG 100H
MOV AL,04H
MOV SI,300H
MOV BL,01
MOV DI,03H
SOL:
    MUL BL
    MOV [SI],AX 
    MOV AL,04
    INC BL
    INC SI
    DEC DI
    CMP DI,0
    JNE SOL
    int 03
    END
    