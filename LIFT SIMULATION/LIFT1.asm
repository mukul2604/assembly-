ORG 100H                  ;;START FROM GND SERVE THE FLOOR AND RETURN GND
MOV BX,0
MOV AL,08BH
MOV DX,0FFFEH  ;;;;;;;PORT MODE INITIALIZATION
OUT DX,AL  

MOV DX,0FFF8H ;;;;;PORT A AS O/P 
MOV AL,0F0H          ;PORT A CONFIGURED AS 11110000
OUT DX,AL

 
MOV DX,0FFFAH  ;;;;;;;PORT B AS INPUT


L1:; IN AL,DX            ;;;;;;;CHECKING FOR REQUEST
    ;AND AL,0FH
    ;JZ L1
 MOV AL,04H

FL:  SHR AL,1            ;;;;;;;;;;;CHECKING FLOOR
     INC BL
     JNC FL
     DEC BL
     CMP BL,0 
     JE ENDER
     
     MOV AL,03
     MUL BL                  ;;; NUMBER OF STEPS STORED IN AX
     XCHG BH,AL
     
     ADD BH,0F0H
     ADD AL,0F0H
     
     MOV DX,0FFF8H 

UP:   OUT DX,AL                 ;;;;;;;;UP ROUTINE
      INC AL
     
     ; MOV CX,0FFH               ;;PUT CX=0FFFE
      ;DELA:LOOP DELA      
     
      CMP BH,AL
      JAE UP
      
DN:     DEC AL                   ;;;;;;;;;;DOWN ROUTINE
        OUT DX,AL
     
        
        ;MOV CX,0FFH         ;;PUT CX=0FFFE
       ; DEL:LOOP DEL
        CMP AL,0F0H
        JBE ENDER
      JMP  DN 
ENDER:OUT DX,AL
      HLT




      
      
      
      
      
   
   
    
