compile:
	g++  .\5stage.cpp .\MIPS_Processoo.hpp -o test1
	g++  .\5stage_bypass.cpp .\MIPS_Processoo.hpp -o test2
	g++  .\79stage.cpp .\MIPS_Processoo.hpp -o test3
	g++  .\79stage_bypass.cpp .\MIPS_Processoo.hpp -o test4

run_5stage :
	./test1 input.asm
run_5stage_bypass :
	./test2 input.asm
run_79stage :
	./test3 input.asm
run_79stage_bypass:
	./test4 input.asm

