ORG 100H                  ;;START FROM GND SERVE THE FLOOR AND RETURN GND
MOV BX,0
MOV AL,08BH
MOV DX,0FFFEH  ;;;;;;;PORT MODE INITIALIZATION
OUT DX,AL
MOV DX,0FFF8H ;;;;;PORT A AS O/P 
MOV AL,0F0H          ;PORT A CONFIGURED AS 11110000
OUT DX,AL
MOV DX,0FFFAH  ;;;;;;;PORT B AS INPUT
L1: ;IN AL,DX
    ;AND AL,0FH
    ;JZ L1
    MOV AL,04
    SHR AL,1
FL: SHR AL,1
    INC BL
    JNC FL
    MOV AL,03
    MUL BL                  ;;; NUMBER OF STEPS STORED IN AX
    XCHG BH,AL
    MOV DX,0FFF8H 
UPFL: OUT DX,AL
      INC AL
      MOV CX,0AAH               
      DELA:LOOP DELA      
      CMP BH,AL
      JAE UPFL
      HLT




      
      
      
      
      
   
   
    
