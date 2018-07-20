/*==========================================================================
 |                   Quectel OpenCPU
 |
 |              Copyright(c) 2012-2013 Quectel Ltd. All rights reserved.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      OpenCPU Initialization.
 |
 |--------------------------------------------------------------------------
 |
 |  Created by   :     Stanley YONG
 |        Date   :     December 17, 2012
 \=========================================================================*/

.file "ocpu_init.S"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.word  __data_load
/* start address for the .data section. defined in linker script */
.word  __data_start
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  __bss_start__
/* end address for the .bss section. defined in linker script */
.word  __bss_end__

.globl _OCPU_InitUserMem
_OCPU_InitUserMem:
	// Copy .data section
	LDR		r0,=__data_load
	LDR		r1,=__data_start
	LDR		r2,=_edata
1:
	CMP		r1,r2
	LDR   r3, [r0], #4
	STR   r3, [r1], #4
	//LDMLTIA	r0!,{r3}
	//STMLTIA	r1!,{r3}

	BLT		1b

	// Zero .bss section
	LDR		r1,=__bss_start__
	LDR		r2,=__bss_end__
2:

	CMP		r1,r2

        MOV   r3, #0
        STR   r3, [r1], #4
	BLT		2b

	BX      lr

