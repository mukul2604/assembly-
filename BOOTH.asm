 ORG 100H
 Y DB 03H ;;;;;;;;;;;;;;;;;;;;CAUTION: NO. OF SWITCHES IN X SHOULD
 X DB 02H                     ;;;;;;;;;;;;;;;;BE LESS THEN Y..
 CTR DB 04H  
 MOV SP,300H
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ;MOV AH,Y
 ;XOR AH,00001111B         ;;;COMPLEMENTING AND SHIFTING FOR VALUE OF U AND V
 
 ;NEG AH 
 ;MOV DL,AH
 
 MOV DH,Y
                         ;;;;;comp y
 ;SHL DL,4                ;;;; y
 SHL DH,4
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 MOV CX,0H
 MOV AH,X
 SHL AH,4
 PUSH AX

SOL: AND AH,00011000B
     MOV BH,AH
     POP AX
     SAR AH,1
     PUSH AX
     DEC CTR
     
    TEST1: CMP BH,0H       ;;;;;;;00000000B
           JNE TEST2
           SAR CX,1
           JMP EOL
    TEST2: CMP BH,10H      ;;;;;;;00010000B
           JNE TEST3
           ;ADD CH,DL 
            SUB CH,DH
           SAR CX,1 
           JMP EOL
    TEST3: CMP BH,08H      ;;;;;;;00001000B
           JNE TEST4
           ADD CH,DH
           SAR CX,1
           JMP EOL  
    TEST4: CMP BH,18H      ;;;;;;;00011000B
           SAR CX,1
           JMP EOL
EOL: CMP CTR,00H
     JNE SOL
    XCHG CL,CH 
         
    ;MOV AH,CL
    ;MOV AL,CH
END