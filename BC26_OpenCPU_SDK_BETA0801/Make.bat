@ECHO OFF
IF "%1" == "" (
	ECHO More parameter is required. 
	ECHO  - "make clean" may clean the compiling, and "make new" may start to compile.
) ELSE (
	CALL make\gcc\gcc_make.bat %1
)
