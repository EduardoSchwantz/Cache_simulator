# Cache_simulator
Trabalho realizado para a disciplina de Arquitetura de Computadores 2, ministrada pelo Professor Dr. MARCELO SCHIAVON PORTO da Universidade Federal de Pelotas

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

		Exemplos de Execução

	Exemplo 1:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_100.bin”:
	./cache_simulator 256 4 1 R 1 bin_100.bin
	O resultado esperado para a saída é: 100 0.9200 0.0800 1.00 0.00 0.00
	
	Exemplo 2:

	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_1000.bin”:
	./cache_simulator 128 2 4 R 1 bin_1000.bin
	O resultado esperado para a saída é: 1000 0.8640 0.1360 1.00 0.00 0.00

	Exemplo 3:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_10000.bin”:
	./cache_simulator 16 2 8 R 1 bin_10000.bin
	O resultado esperado para a saída é: 10000 0.9298 0.0702 0.18 0.79 0.03

	Exemplo 4:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada 
	“vortex.in.sem.persons.bin”:
	./cache_simulator 512 8 2 R 1 vortex.in.sem.persons.bin
	O resultado esperado para a saída é: 186676 0.8782 0.1218 0.05 0.93 0.02
	
	Exemplo 5:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada 
	“vortex.in.sem.persons.bin”:
	./cache_simulator 1 4 32 R 1 vortex.in.sem.persons.bin
	O resultado esperado para a saída é: 186676 0.5440 0.4560 0.00 1.00 0.00

		Exemplos para Random, LRU e FIFO – bin_100.bin:
	Exemplo 6:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_100.bin”:
	./cache_simulator 2 1 8 R 1 bin_100.bin
	O resultado esperado para a saída é: 100 0.43 0.57 0.28 0.68 0.04
	
	Exemplo 7:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_100.bin”:
	./cache_simulator 2 1 8 L 1 bin_100.bin
	O resultado esperado para a saída é: 100 0.46 0.54 0.30 0.67 0.04

	Exemplo 8:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_100.bin”:
	./cache_simulator 2 1 8 F 1 bin_100.bin
	O resultado esperado para a saída é: 100 0.43 0.57 0.28 0.68 0.04
		
		Exemplos para Random, LRU e FIFO – vortex.in.sem.persons.bin:

	Exemplo 9:
	
	Considerando a seguinte linha de comando, utilizando o arquivo de entrada 
	“vortex.in.sem.persons.bin”:
	./cache_simulator 1 4 32 R 1 vortex.in.sem.persons.bin
	O resultado esperado para a saída é: 186676 0.5440 0.4560 0.00 1.00 0.00
	
	Exemplo 10:

	Considerando a seguinte linha de comando, utilizando o arquivo de entrada 
	“vortex.in.sem.persons.bin”:
	./cache_simulator 1 4 32 L 1 vortex.in.sem.persons.bin
	O resultado esperado para a saída é: 186676 0.5756 0.4244 0.00 1.00 0.00

	Exemplo 11:

	Considerando a seguinte linha de comando, utilizando o arquivo de entrada 
	“vortex.in.sem.persons.bin”:
	./cache_simulator 1 4 32 F 1 vortex.in.sem.persons.bin
	O resultado esperado para a saída é: 186676 0.5530 0.4470 0.00 1.00 0.00
