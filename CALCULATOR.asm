ORG 100H
;;;;;;STORAGE1 ENCODED VALUE
MOV SI,500H
MOV [SI+03H],3FH     ;0
MOV [SI+05H],06H     ;1
MOV [SI+09H],5BH     ;2
MOV [SI+11H],4FH     ;3
MOV [SI+21H],66H     ;4
MOV [SI+41H],6DH     ;5
MOV [SI+42H],7DH     ;6
MOV [SI+01H],07H     ;7
MOV [SI+04H],7FH     ;8
MOV [SI+06H],6FH     ;9
MOV [SI+12H],0EH     ; +
MOV [SI+0CH],40H     ; -
MOV [SI+0AH],76H     ; X   

;STORAGE2 ACTUAL VALUE
MOV [44H+SI+03H],0
MOV [44H+SI+05H],1
MOV [44H+SI+09H],2
MOV [44H+SI+11H],3
MOV [44H+SI+21H],4
MOV [44H+SI+41H],5
MOV [44H+SI+42H],6
MOV [44H+SI+01H],7
MOV [44H+SI+04H],8
MOV [44H+SI+06H],9 
MOV [44H+SI+12],0AH     ;;+
MOV [44H+SI+0C],0BH     ;-
MOV [44H+SI+0A],0CH     ;X 

;; STORAGE3
MOV [SI+600H],3FH
MOV [SI+601H],06H
MOV [SI+602H],5BH
MOV [SI+603H],4FH
MOV [SI+604H],66H
MOV [SI+605H],6DH
MOV [SI+606H],7DH
MOV [SI+607H],07H
MOV [SI+608H],7FH
MOV [SI+609H],6FH
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
  MOV AL,0C0H
  OUT DX,AL  
  ;;;;;;;;;;;;;;;;;;;;;;;;;CLEAR
  MOV CX,10H
DEL1:LOOP DEL1       ;DELAY OF 160 uS
 ;;;;;;;;;;;;;;;
  MOV AL,90H   ;;;WRITE DISPLAY RAM CONTROL WORD
  OUT DX,AL

;; KEYBOARD ROUTINE
KEY:    MOV BX,02
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
        
KEYEND: POP BX       ;;;;;BX CONTAINS ROW NUMBER AND AL CONTAINS COLMN NO.
        SHL AL,1 
        ADD BL,AL
        ;;;;;;;;;;;;
        CMP BL,81H
        JNE L3            ;;;;;;;;THIS IS USED TO REDUCE THE INDEX OF DIGIT 6 81 GOES TO AT 42 INDEX
        MOV BL,42H
        ;;;;;;;;;;;;;;;;;;
       
     L3:   MOV AL,[BX+SI]     ;;;AL CONTAINS THE ENCODED BIT
        
        ;;WRITE THE DISPLAY ROUTINE HERE
        MOV DX,0FFE8H
        OUT DX,AL
        ;;;;;;;;;;;;;
        
     ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
        MOV CH,[BX+SI+44H]
        MOV [SI+22H],CH             ;;;[SI+CTR] CONTAINS NUM VALUE
        INC SI
        CMP SI,0502H
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
        
        
            
            
            
            
            
            
        
        
        




