@ECHO OFF
IF "%1" == "" (
	ECHO More parameter is required. 
	ECHO  - "make2 clean" may clean the compiling;
	ECHO  - "make2 test_basic" may start to compile basic function;
	ECHO  - "make2 test_advance" may start to compile basic function;
	ECHO  - "make2 newlib" may start to compile base library.
	ECHO  - "make2 xxx" "xxx" is the project name which you want to make, e.g. "make2 audio"/"make file_stress"/..., use this command, you don't have to modify the "-D xxx" in make\gcc\makefile
) ELSE (
	CALL make\gcc\gcc_test\gcc_make.bat %1
)
