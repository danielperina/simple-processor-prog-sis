#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <deque>
#include <map>
#include <string>

#include "tokens.h"
#include "error_handling.h"

using std::cout;
using std::deque;
using std::map;
using std::string;

#define OPERATOR_SHIFT(a) (a << 12)
#define OPERATOR_MASK(a) (a & 0xf) 
#define OPERAND_MASK(a) (a & 0xfff)

Result<deque<unsigned short>> parse(deque<Token> tokens, bool showlabels) // função de parsing: tokens -> código de maquina
{
    map<string, int> symbols;
	Result<deque<unsigned short>> ret;
	deque<unsigned short> code;
	int mem = 0, current_line = 1;
	string filename = "";

	for(int i = 0; i < tokens.size(); i++) // conta dados, declara símbolos
	{
		switch(tokens[i].type)
		{
			case FILENAME:
			{
				filename = tokens[i].name;
			}break;
			case CURRENT_LINE:
			{
				current_line = tokens[i].value;
			}break;
			case DEFINE_WORD:
			{
				i++;
				while(i < tokens.size() && tokens[i].type == NUMERIC_LITERAL)
				{
					mem++;
					i++;
				}
				i--;
			}break;
			case OPERATOR:
				mem++;
			break;
			case DECLABEL:
			{
				if(symbols.find(tokens[i].name) != symbols.end()) // Se o símbolo já foi declarado
				{
					ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 2));
				}
				else // senão foi declarado
				{
					symbols[tokens[i].name] = mem; // define a posição na memória
				}
			}break;
			// case NEWLINE:
			// break;
		}
	}

	mem = 0;
	current_line = 1;
	filename = "";

	int i = 0;
	unsigned short aux;

    if(showlabels) // mostra os símbolos "labels" se a flag -L foi definida nos arqumentos do ligador
    {
        cout << "\nLabels: {\n";
        for(auto item: symbols)
        {
            cout <<"\t'"<<item.first<<"':"<<item.second<<"\n";
        }
        cout << "}\n";
    }

	while(tokens.front().type != ENDOFPROGRAM) // Faz o parsing e a verificação semântica
	{
        aux = 0;
		switch(tokens.front().type)
		{
			case FILENAME: // armazena o nome do arquivo atual
			{
				current_line = 1;
				filename = tokens.front().name;
				tokens.pop_front();
			}break;
            case ENDOFFILE:
                tokens.pop_front();
            break;
			case CURRENT_LINE:
				current_line = tokens.front().value;
				tokens.pop_front();
			break;
			case NEWLINE:
			{
				tokens.pop_front();
			}break;
			case DECLABEL: // faz a verificação semântica para declaração de labels
			{
				tokens.pop_front();

				if(tokens.front().type != OPERATOR && \
                tokens.front().type != DEFINE_WORD && \
                tokens.front().type != NEWLINE && \
                tokens.front().type != ENDOFFILE && \
                tokens.front().type != ENDOFPROGRAM)
				{
					ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 5));
					while(tokens.front().type != ENDOFPROGRAM)tokens.pop_front();
				}

			}break;
			case OPERATOR: // gera uma instrução e faz a verificação semântica
			{
				aux = 0;
				aux = OPERATOR_SHIFT(OPERATOR_MASK(tokens.front().value));

                if(tokens.front().value == operators["stop"] || tokens.front().value == operators["ret"]) // stop ou ret
				{
                    tokens.pop_front();
                }
                else
                {
                    tokens.pop_front();
    
                    if(tokens.front().type != NUMERIC_LITERAL &&  tokens.front().type != LABEL)
                    {
                        ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 3));
                        while(tokens.front().type != ENDOFPROGRAM)tokens.pop_front();
                    }
                    else
                    {
                        if(tokens.front().type == NUMERIC_LITERAL)
                        {
                            aux |= OPERAND_MASK(tokens.front().value);
                            tokens.pop_front();
                        }
                        else
                        {
                            if(symbols.find(tokens.front().name) == symbols.end())
                            {
                                ret.exceptions.push_back(Exception("'"+tokens.front().name+"'"+string(" found during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 4));
                                tokens.pop_front();
                            }
                            else
                            {
                                aux |= OPERAND_MASK(symbols[tokens.front().name]);
                                tokens.pop_front();
                            }
                        }
                    }
                }

                if(tokens.front().type != NEWLINE && tokens.front().type != ENDOFFILE && tokens.front().type != ENDOFPROGRAM) // verifica se a instrução tem a quantidade de argumentos desejada
                {
                    ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 3));
                    while(tokens.front().type != ENDOFPROGRAM)tokens.pop_front();
                }

				code.push_back(aux);
			}break;
			case DEFINE_WORD: // define dados e faz verificação semântica
			{
				tokens.pop_front();
				while(tokens.front().type == NUMERIC_LITERAL)
				{
					aux = tokens.front().value;
					code.push_back(aux);
					tokens.pop_front();
				}

				if(tokens.front().type != NEWLINE && tokens.front().type != ENDOFFILE && tokens.front().type != ENDOFPROGRAM)
				{
					ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 3));
					while(tokens.front().type != ENDOFPROGRAM)tokens.pop_front();
				}

			}break;
			default:
			{
				ret.exceptions.push_back(Exception(string("during parsing, at ")+filename+string(" line ")+std::to_string(current_line), 5));
				while(tokens.front().type != ENDOFPROGRAM)tokens.pop_front();
			}
		}
	}

	ret.value = code; // armazena o código gerado no result
	return ret;
}

#endif