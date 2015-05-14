;; ***************************************************************************
;; *  $Id: context.inc 272 2011-03-01 20:19:06Z merdmann $
;; *
;; *  Sat Nov 24 18:35:59 2007
;; *  Copyright  2007  Michael Erdmann	
;; *  Email: Michael.Erdmann@snafu.de
;; ****************************************************************************

;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;;

portPUSH MACRO X
	 MOVFF X, POSTDEC1
	 ENDM

portPOP  MACRO X
	 MOVFF PREINC1, X
	 ENDM

;;
;; This macro save the context at the place where the stack parameter
;; is pointing to.
;;	
portSAVE_CONTEXT MACRO( Stack ) 
	portPUSH( WREG )
	portPUSH(STATUS)

	;; Store the necessary registers to the stack.
	portPUSH( BSR )
	portPUSH( FSR2L )
    portPUSH( FSR2H )
	portPUSH( FSR0L )
	portPUSH( FSR0H )
	portPUSH( TABLAT )
	portPUSH( TBLPTRU )
	portPUSH( TBLPTRH )
	portPUSH( TBLPTRL )
	portPUSH( PRODH )
	portPUSH( PRODL )


	;; save the scratchpad from 0..0x020
	CLRF	FSR0L, 0
	CLRF	FSR0H, 0	
	MOVLW	20	
savectx_50:	
		MOVFF	POSTINC0, POSTDEC1   ; push the register on the stack
		DECF	WREG,0	
		BNZ	savectx_50
										
	MOVFF	STKPTR, FSR0L		; save the stack pointer
	MOVFF   STKPTR, WREG

	MOVF WREG,1,0
savectx_100:
	    BZ	savectx_200		; sp = 0; do not do anything 
	
	    portPUSH(TOSL)
	    portPUSH(TOSH)
	    portPUSH(TOSU)

 	    POP
	    DECF    WREG,1
	    GOTO savectx_100

savectx_200:	
	portPUSH(FSR0L)			;  store the call stack pointer 

    MOVFF   FSR1L, Stack
    MOVFF   FSR1H, Stack+1;
  	
	ENDM


;; This is the reverse of portSAVE_CONTEXT.  See portSAVE_CONTEXT for more details.

portRESTORE_CONTEXT MACRO( Stack )
	MOVFF	Stack, FSR1L
	MOVFF	Stack+1, FSR1H

    ;;;  **********************************************************
	;;;  Fill the hardware stack from our software stack. 
	CLRF	STKPTR

	;; How many return addresses are there on the hardware stack?
	portPOP(FSR0L)

	MOVF FSR0L,1,0
rstctx_100:
	    BZ	   rstctx_400
	    
	    PUSH
	    portPOP( WREG )
	    MOVWF       TOSU
	
	    portPOP( WREG )
	    MOVWF       TOSH
			
	    portPOP( WREG )
	    MOVWF       TOSL

	    DECF   FSR0L,1
	    GOTO   rstctx_100

rstctx_400:

	;; restore the scratchpad area from 0..0x020
	LFSR	   FSR0, 0x00001f
	MOVLW	20	
rstctx_450:	
		MOVFF	PREINC1, POSTDEC0
		DECF	WREG,1,0	
		BNZ	rstctx_450
		
	;;  Restore all CPU registers
	portPOP(PRODL)
	portPOP(PRODH)
	portPOP(TBLPTRL)							
	portPOP(TBLPTRH)							
	portPOP(TBLPTRU)
	portPOP(TABLAT)
	portPOP( FSR0H )
	portPOP( FSR0L )
	portPOP( FSR2H )
	portPOP( FSR2L )
	portPOP( BSR )

	portPOP( STATUS	)
	portPOP( WREG )
		
	ENDM
