
ORG 100H
;N DB 9 
MOV AL,07D
MOV SI,300H 
MOV [SI],AL
MOV AL,01D
MOV [SI+1],AL
MOV AL,03D
MOV [SI+2],AL
MOV AL,04H
MOV [SI+3],AL
;

MOV SI,300                             
MOV AH,00H ;;; a
MOV AL,04H   ;;;;int b
MOV DI,500H                                 
RARR DB 20D
K DB 00H        ;;;PIVOT CHOSENfrom start..
RT DB 00D
LT DB 00D
CTR DB 00D


     CMP K,AL          ;;;;if index of pivot>last index
     JA END_P
     
TESTK<B:  CMP K,AL
          JE  CONC
          INC K  
          
TESTSIK<SIA: MOV BH,[SI+K]
             CMP BH,[SI]
             JA TESTELSE
             MOV [DI+LT],BH 
             INC LT
             JMP TESTK<B

TESTELSE:   MOV [DI+RT+RARR],BH
            INC RT
            JMP TESTK<B


CONC: ;;;;;;;;;;;;;;;;;;CONCATENATION

   PUTL:  MOV BL,[DI+CTR]
          MOV [SI],BL
          INC CTR
          INC SI
          MOV BL,CTR
          CMP BL,LT
          JB PUTL
 CTR EQU 0
 
  PUTR: MOV BL,[DI+RARR+CTR] 
        MOV [SI],BL
        INC CTR
        INC SI
        MOV BL,CTR
        CMP BL,RT
        JB PUTR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;        
        






        
                      

 
                    
END_P:RET




