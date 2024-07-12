#ifndef __TOKENS_H__
#define __TOKENS_H__

#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <map>

#include "my_functions.h"
#include "error_handling.h"


using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::deque;
using std::map;

map<string, int> operators = {
	{"la",0x0},
	{"sa",0x1},
	{"aa",0x2},
	{"mul",0x3},
	{"div",0x4},
	{"sub",0x5},
	{"jmp" ,0x6},
	{"jeq",0x7},
	{"jgt",0x8},
	{"jlt"  ,0x9},
	{"pw" ,0xa},
	{"rw" ,0xb},
	{"stop" ,0xc},
	{"jsr", 0xd},
	{"ret", 0xe}
};

enum TokenType
{
	DEFINE_WORD,
	
	NUMERIC_LITERAL,
	
	DECLABEL,
	LABEL,

	OPERATOR,

	FILENAME,
	CURRENT_LINE,
	NEWLINE,
    ENDOFFILE,
	ENDOFPROGRAM,

	ERROR
};

const char * TokenTypeNames[] = {
    "DEFINE_WORD",

	"NUMERICLITERAL",
	
	"DECLABEL",
	"LABEL",

	"OPERATOR",

	"FILENAME",
	"CURRENT_LINE",
    "NEWLINE",
    "ENDOFFILE",
    "ENDOFPROGRAM",

	"ERROR"
};

struct Token
{
	TokenType type;
	string name;
	int value;

	Token(TokenType _type, string _name, int _value)
	{
		type = _type;
		name = _name;
		value = _value;
	}

	Token(TokenType _type, string _name)
	{
		type = _type;
		name = _name;
		value = -1;
	}

	friend std::ostream& operator<<(std::ostream &os, Token &tk);
};

std::ostream& operator<<(std::ostream &os, Token &tk) // função para imprimir tokens
{
	if(tk.value>=0)
	{
		os << "<" << TokenTypeNames[tk.type] << " '" << tk.name << "' " << tk.value << ">";
	}
	else if(tk.name.size()>0)
	{
		os << "<" << TokenTypeNames[tk.type] << " '" << tk.name << "'>";
	}
	else
	{
		os << "<" << TokenTypeNames[tk.type] << ">";
	}

	return os;
}

Result<deque<Token>> tokenize(string src, string filename) // função para gerar os tokens
{
	deque<Token> tokens;
	tokens.push_back(Token(FILENAME, filename));

	Result<deque<Token>> ret;

	tokens.push_back(Token(CURRENT_LINE, "CL", 1));
	int i = 0, current_line = 1;
	while(i < src.size())
	{
        // cout << tokens.back().name << endl;
		switch(src[i])
		{
			case ' ': // ignora espaços e tabulações
			case '\t':
				i++;
			break;
			case '\n': // define os tokens de inicio e fim de linha e conta uma linha
			{
				
				tokens.push_back(Token(NEWLINE, "\\n"));
				current_line++;
				tokens.push_back(Token(CURRENT_LINE, "CL", current_line));
				i++;
			}break;
			case '/': // verifica se é um comentário
			{
				if((i+1) < src.size() && src[i+1]=='/')
				{
					while(src[i]!='\n' && i < src.size())i++;
				}
				else
				{
					tokens.push_back(Token(ERROR, string(1, src[i])));
					ret.exceptions.push_back(Exception("'"+string(1, src[i])+"'"+string(" found during tokenization, at ")+filename+string(" line ")+std::to_string(current_line), 1));
					i++;
				}
			}
			break;
			case '$': // Modificadores de bases numéricas % -> binário, & -> octal, $ -> hexadecimal
			case '%':
			case '&':
			{
				string digit = string(1, src[i]);
				i++;

				while(i<src.size() &&  (std::isdigit(src[i]) || string("abcdef").find(std::tolower(src[i]))!=string::npos))
				{
					digit+=src[i++];
				}

				if(isNumber(digit))
				{
					tokens.push_back(Token(NUMERIC_LITERAL, digit, toInt16(digit)));
				}
				else
				{
					tokens.push_back(Token(ERROR, digit));
					ret.exceptions.push_back(Exception("'"+string(1, digit[0])+"'"+string(" found during tokenization, at ")+filename+string(" line ")+std::to_string(current_line), 0));
				}
			}break;
			default:
			{
				if(std::isdigit(src[i])) // verifica se é número
				{
					string digit;
					
					while(i<src.size() && std::isdigit(src[i]))
					{
						digit += src[i++];
					}

					tokens.push_back(Token(NUMERIC_LITERAL, digit, toInt16(digit)));
				}
				else if(std::isalpha(src[i])) // verifica se é operador, identificador ou diretiva do ligador
				{
					string identifier;

					while(i < src.size() && (std::isalpha(src[i]) || std::isdigit(src[i])))
					{
						identifier += src[i++];
					}

					string idcpy = identifier;

					for(int j=0; j<idcpy.size();j++)
						idcpy[j] = std::tolower(idcpy[j]);
					
					if(operators.find(idcpy)!=operators.end())
					{
						tokens.push_back(Token(OPERATOR, identifier, operators[idcpy]));
					}
					else if(idcpy=="dw")
					{
						tokens.push_back(Token(DEFINE_WORD, identifier));
					}
					else
					{
						if(i < src.size() && src[i]==':')
						{
							tokens.push_back(Token(DECLABEL, identifier));
							i++;
						}
						else
						{
							tokens.push_back(Token(LABEL, identifier));
						}
					}
				}
				else
				{
					tokens.push_back(Token(ERROR, string(1, src[i])));
					ret.exceptions.push_back(Exception("'"+string(1, src[i])+"' "+string("found during tokenization, at ")+filename+string(" line ")+std::to_string(current_line), 1));
                    i++;
				}
			}
		}
	}

    if(tokens.back().type != NEWLINE)tokens.push_back(Token(NEWLINE, "\\n")); // se o arquivo não termina com '\n' adiciona
    
    tokens.push_back(Token(ENDOFFILE, "EOF")); // adiciona o token ENDOFFILE
	ret.value = tokens;
	return ret;
}

#endif