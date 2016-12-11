ORG 100H                  ;;START FROM GND SERVE THE FLOOR AND RETURN GND
MOV BX,0
MOV AL,08BH
MOV DX,0FFFEH  ;;;;;;;PORT MODE INITIALIZATION
OUT DX,AL
MOV DX,0FFF8H ;;;;;PORT A AS O/P 
MOV AL,0F0H          ;PORT A CONFIGURED AS 11110000
OUT DX,AL
MOV DX,0FFFAH  ;;;;;;;PORT B AS INPUT
L1:; IN AL,DX
    ;AND AL,0FH
    ;JZ L1
    MOV AL,04H
    SHR AL,1
    JMP FL

L2:  IN AL,DX
     INC BH
     CMP BH,04H
     JE UPFL
     AND AL,0FH
     PUSH AX
     JZ L2   
     JMP RETU

FL:  SHR AL,1
     INC BL
     JNC FL
     MOV AL,03
     MUL BL                  ;;; NUMBER OF STEPS STORED IN AX
     XCHG AH,AL
     MOV DX,0FFF8H 

UPFL: OUT DX,AL   
      INC AL
      PUSH AX
      JMP L2
RETU: MOV CX,01H
      DELAY:LOOP DELAY
      CMP AH,AL
      POP AX
      JAE UPFL
      DEC AL        
      
      
      
      
DNFL: OUT DX,AL
      DEC AL  
      JZ ENDER
      MOV CX,01H
      DEL:LOOP DEL
      JMP  DNFL 
ENDER:OUT DX,AL
      HLT




      
      
      
      
      
   
   
    
