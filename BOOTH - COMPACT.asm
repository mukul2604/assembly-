 ORG 100H

 CTR DB 04H  
 
 MOV CX,0H
 MOV DH,07
 SHL DH,4
 MOV AH,04
 SHL AH,4
 PUSH AX
SOL: AND AH,18H
     MOV BH,AH   
     
     
     POP AX
     SHR AH,1
     PUSH AX
     DEC CTR
   
    TEST1: CMP BH,0H       ;;;;;;;00000000B
           JNE TEST2
           SAR CX,1
           JMP EOL
    TEST2: CMP BH,10H      ;;;;;;;00010000B
           JNE TEST3
           SUB CH,DH
           SAR CX,1
           ;JO LABEL 
           JMP EOL
    TEST3: CMP BH,08H      ;;;;;;;00001000B
           JNE TEST4
           ADD CH,DH
           SAR CX,1
           ;JO LABEL
           JMP EOL  
    TEST4: CMP BH,18H      ;;;;;;;00011000B
           SAR CX,1
           ;JO LABEL
           JMP EOL
            ;LABEL: PRINT 'OVERFLOW'
EOL: CMP CTR,00H
     JNE SOL
      
             
     
END                
