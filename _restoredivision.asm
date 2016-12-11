 ORG 100H
 REM DB 00H 
 CTR DB 08H 
 NUM DB 08H
 DEN DB 02H    ;;;;;;;;;;;DIVISOR
 MOV SP,300H
 CLC
 
  MOV AH,NUM         ;;;;;DIVIDEND
  MOV AL,REM         ;;;;;REMAINDER
SOL:  
      ROL AH,1                 ;;;AH==DIVIDEND,AL==REMAINDER
      ROL AL,1
      PUSH AX
      ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;1ST STEPDONE NUM AND REM ARE SHIFTED LEFT
      
      ROL AH,1
      AND AH,01H   
      CMP AH,01H
      JNE NOPE
      OR AL,01H
NOPE: AND AL,11111110B
      ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;2ND STEP MSB OF DIVIDEND==>LSB REM
      
      MOV CL,AL        ;;;;;;CL ==REMAINDER  TEMP VARIABLE 
      ;;;;;;;;;;;;;;;;;;;;;;;;;;3RD STEP
                                                      
      SUB CL,DEN             ;;;;;;;;REMAINDER-DIVISOR
      ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;4TH STEP
      MOV AL,00H
      ADC AL,00H  ;;;;;;;;;;;CARRY CHECK
      
      CMP AL,01H        ;;;CONDN CHECK
      JNE RESPOS
      
RESNEG: POP AX                ;;;RESULT NEGATIVE
        MOV AL,CL
        AND AH,11111110B
        JMP FINAL

RESPOS: POP AX
        MOV AL,CL
        OR AH,01H
        JMP FINAL
        
FINAL:  DEC CTR
        JNZ SOL
        
        END

   
      
      
 
 
 
 
 