ORG 100H
;;;;;;STORAGE1 ENCODED VALUE
MOV SI,300H

MOV CL,0  
OUTLOOP:MOV BP,01
LS:MOV AL,2  
MOV AH,0
 MOV [BP+SI],CL
 INC CL
 MUL BP
 MOV BP,AX 
 CMP CL, 0DH     ;2ND LOOP ONLY
 JE OUT          ;2ND LOOP ONLY
 CMP CL,08H
 JNE LS 
 ADD SI,100H
 JMP OUTLOOP


;STORAGE2 ACTUAL VALUE 
OUT:MOV  AH,3FH   
MOV [SI+60H],AH
MOV  AH,06H
MOV [SI+61H],AH
MOV  AH,5BH
MOV [SI+62H],AH
MOV  AH,4FH
MOV [SI+63H],AH
MOV  AH,66H
MOV [SI+64H],AH
MOV  AH,6DH
MOV [SI+65H],AH
MOV  AH,7DH
MOV [SI+66H],AH
MOV  AH,07H
MOV [SI+67H],AH
MOV  AH,7FH
MOV [SI+68H],AH
MOV  AH,6FH
MOV [SI+69H],AH    

  ;;;;;;;;;;;;;INITIALIZATION
  ;8255
  MOV DX,0FFFEH
  MOV AL,9AH
  OUT DX,AL
  ;8279
  MOV DX,0FFEAH
  MOV AL,10H    
  OUT DX,AL
  ;;;;;;;;;;;;;;;;;;;;;;;;     ;MODE SET
  MOV AL,38H
  OUT DX,AL  
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;CLOCK SET DIVIDED BY 24
  
  ;;;;;;;;;;;;;;;;;;;;;;;;;CLEAR
 MOV BP,41H
 ;;;;;;;;;;;;;;;
  

;; KEYBOARD ROUTINE
KEY:    MOV AL,0C0H
        OUT DX,AL  
        MOV CX,10H
    DEL1:LOOP DEL1       ;DELAY OF 160 uS


        MOV BX,02
     L2:MOV AL,01
     L1:MOV DX,0FFFCH
        OUT DX,AL
        MOV DX,0FFF8H
        PUSH AX
        MOV AL,0
        IN AL,DX
        CMP AL,0                ;FIRST CHECK
        JE L1
        ;;;;;;;;;;;;;;;;;;;;;;;;
        MOV CX,50H
        DELAY:LOOP DELAY
       ;;;;;;;;;;;;;;;;;;;;;;;; 
        IN AL,DX
        CMP AL,0                 ;;;;;;;;;;SECOND CHECK
        JNE KEYEND
       ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        POP AX
        MUL BL
        CMP AL,04
        JBE L1
        JMP L2
        MOV BX,0
KEYEND: MOV BL,AL
        POP CX       ;;;;;BX CONTAINS ROW NUMBER AND AL CONTAINS COLMN NO.
        CMP CL,01
        JNE SECND
        MOV AL,[BX+SI]
 SECND: MOV AL,[BX+SI+100H]
       
        MOV [BP+SI],AL    ;;;AL CONTAINS OPERAND HERE WHICH IS STORED TO [BP+SI]
        INC BP
        
        MOV BL,AL    
        ADD BX,60H
        MOV AL,[BX+SI] 
        
        
        
        
        
 
 
 
    ;;;;;;;;;;;;;WRITE THE DISPLAY ROUTINE HERE
        MOV DX,0FFEAH
        MOV AL,90H   ;;;WRITE DISPLAY RAM CONTROL WORD
        OUT DX,AL
        MOV DX,0FFE8H
        OUT DX,AL
        
        ;;;;;;;;;;;;;
        
     ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
        
        JBE KEY
      ;;;;;;;;;;;;;;;;;;;;;;;;;;;
     
         MOV SI,500H
   CALC: MOV AX,[SI+22H]
         MOV CX,[SI+23H]
         MOV DX,[SI+24H]
         
    PLUS:CMP CX,0AH   ;; ROUTINE PLUS
         JNE SUBT
         ADD AX,DX   
         JMP ENDER

    SUBT:CMP CX,0BH
         JNE MULT
         SUB AX,DX 
         JMP ENDER
  
   MULT: CMP CX,0CH
         MUL DX
       
       
       
  ENDER:  MOV BL,0AH     ;;;;;;;;;;;;;;SPLITTING THE NUMBER
          DIV BL
          MOV CX,AX
        ;;; CLEAR THE DISPLAY RAM ;;;;AH CONTAIN REMAINDER i.e. unit no. and AL CONTAIN 10TH NO.
          MOV DX,0FFEAH
          MOV AL,0C0H
          OUT DX,AL
         
                  ;;;;;;;STARTING FROM ZERO INDEX
         ;;;;;;;;;;;;;;;;;;;
            MOV DH,50H
       DEL: DEC DH
            CMP DH,0
            JNE DEL            ;MAKE SURE 160 uS DELAY
         
       ;;;;;;;;;;;;;;;;;;
          
          MOV AL,90H        ;MODE SET AGAIN
          OUT DX,AL          
       
       ;;;;;;;;;;;;;;;;;;;;;;;;;; ENCODING THE DIGITS   
          MOV BX,0
          MOV BL,CH
          MOV CH,[BX+SI+600H]
          MOV BL,CL
          MOV CL,[BX+SI+600H]
          
       ;;;;;;;;;;;;;;;;;;;;;;  
        MOV AL,CH
        MOV DX,0FFE8H
        OUT DX,AL
       ;;;;;;;;;;;;;
        MOV AL,CL
        OUT DX,AL
       ;;;;;;;;;;;;;;;;
        INT 03 
        END
        
        
            
            
            
            
            
            
        
        
        




