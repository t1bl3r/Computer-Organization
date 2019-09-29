; Begin reserved section: do not change ANYTHING in reserved section!
; The ONLY exception to this is that you MAY change the .FILL values for
; Option, Value1 and Value2. This makes it easy to initialize the values in the
; program, so that you do not need to continually re-enter them. This
; makes debugging easier as you need only change your code and re-assemble.
; Your test value(s) will already be set.
;------------------------------------------------------------------------------
; Author: Fritz Sieker
;
; Description: Tests the implementation of a simple string library and I/O
;

            .ORIG x3000
            BR Main
Functions
            .FILL Test_pack         ; (option 0)
            .FILL Test_unpack       ; (option 1)
            .FILL Test_printCC      ; (option 2)
            .FILL Test_strlen       ; (option 3)
            .FILL Test_strcpy       ; (option 4)
            .FILL Test_strcat       ; (option 5)
            .FILL Test_strcmp       ; (option 6)

; Parameters and return values for all functions
Option      .FILL 0                 ; which function to call
String1     .FILL x4000             ; default location of 1st string
String2     .FILL x4100             ; default location of 2nd string
Result      .BLKW 1                 ; space to store result
Value1      .FILL 0                 ; used for testing pack/unpack
Value2      .FILL 0                 ; used for testing pack/unpack
lowerMask   .FILL 0x00FF            ; mask for lower 8 bits
upperMask   .FILL 0xFF00            ; mask for upper 8 bits

Main        LEA R0,Functions        ; get base of jump table
            LD  R1,Option           ; get option to use, no error checking
            ADD R0,R0,R1            ; add index of array
            LDR R0,R0,#0            ; get address to test
            JMP R0                  ; execute test

Test_pack   
            LD R0,Value1            ; load first character
            LD R1,Value2            ; load second character
            JSR pack                ; pack characters
            ST R0,Result            ; save packed result
End_pack    HALT                    ; done - examine Result

Test_unpack 
            LD R0,Value1            ; value to unpack
            JSR unpack              ; test unpack
            ST R0,Value1            ; save upper 8 bits
            ST R1,Value2            ; save lower 8 bits
End_unpack  HALT                    ; done - examine Value1 and Value2

Test_printCC    
            LD R0,Value1            ; get the test value
            .ZERO R1                ; reset condition codes
            JSR printCC             ; print condition code
End_printCC HALT                    ; done - examine output

Test_strlen 
            LD R0,String1           ; load string pointer
            GETS                    ; get string
            LD R0,String1           ; load string pointer
            JSR strlen              ; calculate length
            ST R0,Result            ; save result
End_strlen  HALT                    ; done - examine memory[Result]

Test_strcpy 
            LD R0,String1           ; load string pointer
            GETS                    ; get string
            LD R0,String2           ; R0 is dest
            LD R1,String1           ; R1 is src
            JSR strcpy              ; copy string
            PUTS                    ; print result of strcpy
            NEWLN                   ; add newline
End_strcpy  HALT                    ; done - examine output

Test_strcat 
            LD R0,String1           ; load first pointer
            GETS                    ; get first string
            LD R0,String2           ; load second pointer
            GETS                    ; get second string
            LD R0,String1           ; dest is first string
            LD R1,String2           ; src is second string
            JSR strcat              ; concatenate string
            PUTS                    ; print result of strcat
            NEWLN                   ; add newline
End_strcat  HALT                    ; done - examine output

Test_strcmp 
            LD R0,String1           ; load first pointer
            GETS                    ; get first string
            LD R0,String2           ; load second pointer
            GETS                    ; get second string
            LD R0,String1           ; dest is first string
            LD R1,String2           ; src is second string
            JSR strcmp              ; compare strings
            JSR printCC             ; print result of strcmp
End_strcmp  HALT                    ; done - examine output

;------------------------------------------------------------------------------
; End of reserved section
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; on entry R0 contains first value, R1 contains second value
; on exit  R0 = (R0 << 8) | (R1 & 0xFF)

pack       AND R4,R4,#0         ; clear R4 
           LD R4,lowerMask      ; 
           AND R0,R0,R4         ; clear
           AND R1,R1,R4         ; should rid both values of their top 8 bits
           AND R2,R2,#0         ; set R2 to 0 for counter
           ADD R2,R2,#8         ; loop counter
           BRp LOOP             ; enter dat loop
      LOOP ADD R0,R0,R0         ; shift one to the left
           ADD R2,R2,#-1        ; decrement counter
           BRp LOOP             ; if it's still positive keep going 
           NOT R0,R0            ; DeMorgan's law
           NOT R1,R1            ; 
           AND R0,R1,R0         ; and the nots
           NOT R0,R0            ; should give us the or of orig R0 and R1
            RET

;------------------------------------------------------------------------------
; on entry R0 contains a value
; on exit  (see instructions
unpack     AND R1,R1,#0             ; 
           LD R1,lowerMask          ; 
           AND R1,R1,R0             ; set R1
           AND R2,R2,#0             ; 
           LD R2,upperMask          ; 
           AND R0,R0,R2             ; 
           AND R2,R2,#0             ; destination mask 
           ADD R2,R2,#1             ; 
           AND R3,R3,#0             ; source mask
           ADD R3,R3,#1             ;
           AND R4,R4,#0             ; 
           ADD R4,R4,#8             ; 
      mask ADD R3,R3,R3             ;  
           ADD R4,R4,#-1            ; 
           BRp mask                 ; 
    
     check AND R4,R3,R0             ; check
           BRp addIt                ; 
           AND R4,R4,#0             ; 
           ADD R2,R2,R2             ; 
           ADD R3,R3,R3             ; 
           BRn EXIT                 ; 
           BR check                 ; 
     addIt ADD R0,R0,R2             ; add to R0
           AND R4,R4,#0             ; clear check again
           ADD R2,R2,R2             ; bit checker 
           ADD R3,R3,R3             ; checker 
           BRn EXIT                 ; 
           BR check                 ; 
      EXIT 
           AND R4,R4,#0             ; 
           LD R4,lowerMask          ; 
           AND R0,R4,R0             ; 
           RET



;------------------------------------------------------------------------------
; on entry R0 contains value
; on exit  (see instructions)

StringNEG   .STRINGZ "NEGATIVE\n"   ; output strings
StringZERO  .STRINGZ "ZERO\n"       ;
StringPOS   .STRINGZ "POSITIVE\n"   ;
StringRet .FILL x0
 
printCC ST R7,StringRet             ;
        AND R1,R1,#0                ; 
        ADD R1,R1,R0                ;     
        ADD R0,R0,#0                ; 
                                         
        BRP printP		    ;					   
        BRZ printZ		    ;					
        BRN printN           	    ;	     
        
printZ
        LEA R0,StringZERO           ;
        PUTS                        ;
	BR last                     ;
printP 
        LEA R0,StringPOS            ;
        PUTS  			    ;
	BR last                     ;
printN
        LEA R0,StringNEG            ;
        PUTS                        ;
	BR last                     ;
		
last	LD R7,StringRet             ;
        AND R0,R0,#0                ;
        ADD R0,R0,R1                ; 
         RET
 
 

;------------------------------------------------------------------------------
; size_t strlen(char *s)
; on entry R0 points to string
; on exit  (see instructions)

strlen      AND R1,R1,#0             ; 
            AND R2,R2,#0             ; 
      LOOP3 LDR R3,R0,#0             ; 
            Brz EXIT2                ; 
            ADD R2,R2,#1             ; 
            ADD R0,R0,#1             ; 
            BR LOOP3                 ;  
      EXIT2 AND R0,R0,#0             ; 
            ADD R0,R0,R2             ;  
            RET

;------------------------------------------------------------------------------
; char *strcpy(char *dest, char *src)
; on entry R0 points to destination string, R1 points to source string
; on exit  (see instructions)
holdIt .FILL x0                      ;
strcpy      
            AND R3,R3,#0             ; 
            ST R0,holdIt             ; 
      
       Circ LDR R3,R1,#0             ; 
            STR R3,R0,#0             ; 
            
            ADD R1,R1,#1             ; 
            ADD R0,R0,#1             ;
  
            ADD R3,R3,#0             ; 
            BRnp Circ                ; 
          
            LD R0,holdIt             ;
             RET                     ; 


;------------------------------------------------------------------------------
; char *strcat(char *dest, char *src)
; on entry R0 points to destination string, R1 points to source string
; on exit  (see instructions)

strcat_RA   .BLKW 1                 ; space for return address
strcat_dest .BLKW 1                 ; space for dest
strcat_src  .BLKW 1                 ; space for src

strcat      ST R7,strcat_RA         ; save return address
            ST R0,strcat_dest       ; save dest
            ST R1,strcat_src        ; save src

            AND R2,R2,0             ; 
            AND R3,R3,0             ;
        
        end
            LDR R3,R0,0             ;
            BRz copyDat             ;
            ADD R0,R0,1             ;
            BRp end                 ;
		
    copyDat
            LDR R2,R1,0             ;
            STR R2,R0,0             ;
            BRz done                ;
            ADD R0,R0,1	            ;
            ADD R1,R1,1		    ;
            BRp copyDat             ;	
  
       done LD R0,strcat_dest       ; restore dest
            LD R7,strcat_RA         ; restore return address
            RET

;------------------------------------------------------------------------------
; int strcmp(char *s1, char *s2)
; on entry R0 points to first string, R1 points to second string
; on exit  (see instructions)
countC .FILL x0 
strcmp_RA    .BLKW 1                ; 
                 
strcmp          ST R7,strcmp_RA     ;
                AND R2,R2,0         ;initialize holder for R0/s1 char
		AND R3,R3,0         ;initialize holder for R1/s2 char
                AND R4,R4,0	    ;initialize counter to 0

          loopy		
		AND R7,R7,0         ;intialize not of R1 char
		LDR R2,R0,0         ;load R0 into R2
		LDR R3,R1,0         ;load R1 into R3
		
		NOT R7,R3           ;not of current R1 char
		ADD R7,R7,1
		ADD R4,R2,R7	    ;subtract difference of chars
	        BRnp endIt	
		ADD R0,R0,1
		ADD R1,R1,1         ; 
                ADD R2,R2,0         ;
                BRz checkIt         ;
                ADD R3,R3,0         ;
                BRz checkOther      ;
		BRnzp loopy         ;else go back through loop
	
        checkIt	ADD R3,R3,0         ; check if the other is also null 
                BRz endIt           ;
                BR loopy            ;
     checkOther ADD R2,R2,0         ; 
                BRz endIt           ;
                BR loopy            ; 
         endIt		
		ST R4,countC        ;
                LD R7,strcmp_RA     ;
		LD R0,countC        ;
            RET

;------------------------------------------------------------------------------
            .END
