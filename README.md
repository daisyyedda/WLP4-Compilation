# WLP4-to-MIPS-Compiler
A simulation of the WLP4 (a simplified version of C++) compilation process, which includes the followings: <br/>
* A [compiler](https://github.com/daisyyedda/WLP4-to-MIPS-Compiler/blob/fbae859c3e83a29dfd9d330a1adc9ca2167cc65e/a8/a8p5/wlp4gen.cc) that translates WLP4 programs into MIPS Assembly Language. <br/>
* An [assembler] (https://github.com/daisyyedda/WLP4-to-MIPS-Compiler/blob/fbae859c3e83a29dfd9d330a1adc9ca2167cc65e/a3/asm.cc) that translates MIPS Assembly Language to MERL (MIPS Executable Relocatable Linkable) object code. <br/>
* A [linker/loader](https://github.com/daisyyedda/WLP4-to-MIPS-Compiler/blob/fbae859c3e83a29dfd9d330a1adc9ca2167cc65e/a8/a8p7/load.asm) that links the MERL object files, any needed library code, and a small runtime file to form an executable program. <br/>
