#!/bin/bash

# Compila os arquivos fonte C++ utilizando g++
g++ -std=c++20 ./src/error_handling.cpp ./src/vm_link.cpp -o ./bin/vm_link

# Verifica o código de saída da compilação
if [ $? -eq 0 ]; then
    # Executa o binário se a compilação tiver sido bem sucedida
    ./bin/vm_link -m./samples/test.s -l./samples/test0.s -P
#else
#    echo "Compilação falhou."
fi
