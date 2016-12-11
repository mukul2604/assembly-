ORG 100H

MAIN: N DW 4
      MOV AX,N
      SUB SP,2
      PUSH AX
      CALL FACT
      ADD SP,2
      POP AX
      INT 01
      FACT PROC NEAR
        PUSH BP
        MOV BP,SP
        PUSH AX
        PUSH BX
        PUSH DX
        MOV BX,[BP+4]
        MOV AX,1
        CMP AX,BX
        JAE TERM
        DEC BX
        SUB SP,2
        PUSH BX
        CALL FACT
        ADD SP,2
        POP AX
        MOV BX,[BP+4]
        MUL BX
        
     TERM: MOV [BP+6],AX
            POP DX
            POP BX
            POP AX
            MOV SP,BP
            POP BP
            
            RET