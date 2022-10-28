Projeto desenvolvido por:

Daniel Luís nº 56362
João Matos nº 56292
João Santos nº 57103

-----------------------------------------------------------------------------------------------------

É utilizada a API POSIX, pelo que deve ser apenas utilizado em sistemas operativos UNIX.

O Makefile fornecido (executado através do comando "make") compila os ficheiros com a flag -g, 
de forma facilitar o debug dos mesmos, -lrt para permitir a utilização da função shm_open da 
POSIX e -Wall para alertar sobre os possíveis erros.
No mesmo Makefile foi ainda incluída uma função clean (executada com o comando "make clean"), que
remove todo o conteúdo gerado ao compilar os ficheiros c.

Foi também utilizada a função valgrind e verificou-se que não existem
quaisquer memory leaks.