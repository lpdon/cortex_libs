/* Thumb-2 instructions are only supported in unified syntax mode */
 .syntax unified
 /* Put these codes into .text section */
 .section ".text"
 /* Declare main() as global..Otherwise the linker won't find it */
 .global fir
 /* Declare as thumb function..Otherwise it may not linked correctly */
 .thumb_func
//PUBLIC  __iar_program_start
  
    #define baseEntrada  r0
 	#define tamEntrada r1
 	#define baseFiltro r2
 	#define tamFiltro r3
 	#define baseSaida r4
 	#define n r5
 	#define i r6
 	#define paramEntrada r7
 	#define paramFiltro r8        
    #define paramSaida r12
 	#define endEntrada r9
 	#define endFiltro r10
 	#define endSaida r11

        SECTION .text : CODE (2)
        THUMB
//__iar_program_start:
fir:
                push {r4-r12}
                ldr baseSaida, [sp, #36]
                mov n, #0
                mov i, #0
loop1:
                mov i, #0
                mov paramSaida, #0
loop2:
                mov endEntrada, n, lsl #2 //n*4
                add endSaida, baseSaida, endEntrada //baseSaida + n*4
                
                mov endFiltro, i, lsl #2
                
                sub endEntrada, endEntrada, endFiltro //n*4-i*4
                add endEntrada, baseEntrada, endEntrada //baseEntrada + n*4
                ldr paramEntrada, [endEntrada]
                
                add endFiltro, baseFiltro, endFiltro //baseFiltro + i*4
                ldr paramFiltro, [endFiltro]
                
                mla paramSaida, paramFiltro, paramEntrada, paramSaida //y[n] = b[i]*x[n-i]
                str paramSaida, [endSaida]                
                
                add i, i, #1
                cmp i, tamFiltro
                bne loop2
                
                add n, n, #1
                cmp n, tamEntrada
                bne loop1
end:
                pop {r4-r12}
		mov pc, lr
                END

