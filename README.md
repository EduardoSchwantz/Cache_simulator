# Cache_simulator
Trabalho realizado em conjunto da colega Cecilia Botelho para a disciplina de Arquitetura de Computadores 2, ministrada pelo Professor Dr. MARCELO SCHIAVON PORTO da Universidade Federal de Pelotas

	git clone https://github.com/EduardoSchwantz/Cache_simulator.git


	----------------------------Execução----------------------------------------------------

		TODOS ARQUIVOS DEVEM ESTAR NO MESMO DIRETÓRIO
    

	Execução em Windows

	    Compilação
		    gcc cache_simulator.c -o cache_simulator.exe -Wall
	    Execução
		    ./cache_simulator.exe <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada.bin

	Execução em Linux

	Modificações
	
		Retirar
    		linha  7 -#include <windows.h>
		linha 48 - SetConsoleOutputCP(65001);
    
	    Compilação
		    gcc cache_simulator.c -o cache_simulator.exe -Wall -lm 
      
	    Execução
		    ./cache_simulator.exe <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada.bin
