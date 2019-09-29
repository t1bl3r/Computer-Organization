; Author: Fritz Sieker
;
; ------------------------------------------------------------------------
; Begin reserved section - do not change ANYTHING is this section

               .ORIG x3000
               BR Main

option         .BLKW 1          ; select routine to test
data1          .BLKW 1          ; use ONLY for testing
data2          .BLKW 1          ; use ONLY for testing

stackBase      .FILL X4000      ; initial sttack pointer

Main           LD R6,stackBase  ; initialize stack pointer
               LD R0,option     ; select routine to test
               BRZ testPrintNum ; option = 0 means test printNum

               ADD R0,R0,#-1
               BRZ testGetDigit ; option = 1 means test getDidit

               ADD R0,R0,#-1
               BRZ testDivRem   ; option = 2 means test divRem

               HALT             ; invalid option if here

testPrintNum                    ; call printNum(x, base)
               LD R0,data2
               PUSH R0          ; push base argument
               LD R0,data1
               PUSH R0          ; push value argument
               JSR printNum
               ADD R6,R6,#2     ; caller clean up 2 parameters
               BR endTest

testGetDigit                    ; call getChar(val)
               LD R0,data1
               PUSH R0          ; push argument (val to convert to ASCII)
               JSR getDigit     ; 
               POP R0           ; get the corresponding character
               ADD R6,R6,#1     ; caller clean up 1 parameter
               OUT              ; print the digit
               NEWLN
               BR endTest

testDivRem                      ; call divRem(num, denom, *quotient, *remainder)
               LEA R0,data2     ; get pointer to remainder (&data2)
               PUSH R0
               LEA R0,data1     ; get pointer to quotient (&data1)
               PUSH R0
               LD R0,data2
               PUSH R0          ; push demoninator
               LD R0,data1
               PUSH R0          ; push numerator
               JSR divRem       ; call routine
               ADD R6,R6,#4     ; caller clean up 4 parameters

endTest        LEA R0,endMsg
               PUTS
theEnd         HALT             ; look at data1/data2 for quotient/remainder

                                ; useful constants
endMsg         .STRINGz "\nTest complete!\n"

negSign        .FILL    x2D     ; ASCII '-'
digits         .STRINGZ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" ; up to base 36

; end reserved section

; ------------------------------------------------------------------------
; Author: your name goes here
; ------------------------------------------------------------------------
;
; C declaration: char getDigit (int val);

getDigit      ADD R6,R6,#-1     ; callee setup
              PUSH R7           ; make space for return value  
              PUSH R5           ; save return address
              ADD R5,R6,#-1     ; save caller's frame pointerl
             
              LEA R1,digits     ; load pointer to digits
              LDR R0,R5,#4      ; load value  
              ADD R0,R1,R0      ;
              LDR R1,R0,#0      ;  
              STR R1,R6,#2      ; 
       
              POP R5            ; 
              POP R7            ; code for getDigit
                                ; callee cleanup
               RET              ; return
; ------------------------------------------------------------------------
;
; C declaration: void divRem(int num, int denom, int*quotient, int*remainder);

divRem   
         ; LDR R0,R6,#2          ;
         ; LDR R1,R0,#0          ; 
         ; ADD R1,R1,#1          ; 
         ; STR R1,R0,#0          ; 
         ; RET                   ;
 
          PUSH R7               ;
          PUSH R5               ;
          ADD R5,R6,#-1         ;  
          ADD R6,R6,#-1         ; 
 
Check1    LDR R0,R6,#6          ; loading pointer address 
          LDR R2,R6,#5          ; 
          LDR R1,R6,#4          ; loading denom  
          LDR R4,R6,#3          ; 
          NOT R1,R1             ; 
          ADD R1,R1,#1          ; 
         
          AND R3,R3,#0          ; counter for quotient  	 
Loop1     ADD R4,R4,R1          ; 
          BRn end1              ; 
          ADD R3,R3,#1          ; count 1 
          ADD R4,R4,#0          ; 
          BRz end2              ; if zero new step
          ADD R1,R1,#0          ; 
          BRz end2              ; 
          BR loop1              ; 
end2      STR R3,R2,#0          ; store normal quotient
          AND R3,R3,#0          ; set a 0
          STR R4,R0,#0          ; set remainder to 0
          ADD R6,R6,#1          ; 
          POP R5                
          POP R7
          RET
end1      
          STR R3,R2,#0          ; store quotient 
          NOT R1,R1             ; flip back
          ADD R1,R1,#1          ; 
          ADD R4,R4,R1          ; 
          STR R4,R0,#0          ; store remainder 
          ADD R6,R6,#1          ;  
          POP R5                ; 
          POP R7                ;           
                                ; callee cleanup
          RET                   ; return
; ------------------------------------------------------------------------
;
; C declaration: void printNum (int val, int base);

printNum     PUSH R7            ;
             PUSH R5            ; callee setup
             ADD R6,R6,#-2      ;
             ADD R5,R6,#1       ;
             LDR R0,R5,#3       ; 
             ADD R0,R0,#0       ;
             BRzp next          ; 
             NOT R0,R0          ;
             ADD R0,R0,#1       ; flip 
             STR R0,R5,#3       ; store back 
             LD R0,negSign      ; 
             OUT                ; 

next         ADD R5,R5,#-1      ; pushing the pointer for divRem 
             PUSH R5            ; 
             ADD R5,R5,#1       ; 
             PUSH R5            ;  
             LDR R0,R5,#4       ;
             PUSH R0            ;
             LDR R0,R5,#3       ;
             PUSH R0            ; 
             JSR divRem         ; if not single digit, find div and rem   
             ADD R6,R6,#4       ; get to quot  
             LDR R0,R5,#0       ; 
             ADD R0,R0,#0       ; 
             BRz print          ; 
             LDR R0,R5,#4       ; loading base
             PUSH R0            ; 
             LDR R0,R5,#0       ; load quotient
             PUSH R0            ; 
             JSR printNum       ; run again until 0 in quotient   

print        LDR R0,R5,#-1      ; load remainder 
             PUSH R0
             JSR getDigit       ; get correct digit 
             POP R0             ; 
             ADD R6,R6,#1       ; 
             OUT                ; print back to console   

             ADD R6,R6,#2       ; clean stack
             POP R5             ; callee cleanup
             POP R7             ; return address
             ADD R6,R6,#2       ;  
               RET              ; return
; ------------------------------------------------------------------------
               .END
