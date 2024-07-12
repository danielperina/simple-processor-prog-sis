#include <iostream> // cin, cout, cerr, end
#include <deque> // deque<>
#include <map> // map<>
#include <vector> // vector<>
#include <string> // string, string::npos, ...
#include <fstream> // ifstream, ofstream
#include <iomanip> // setw, setfill
#include <algorithm> // find_if()

#include "tokens.h" // tokenize, ...
#include "parser.h" // parse, ...

// #include "error_handling.h"

using std::cout;
using std::endl;
using std::string;
using std::setw;
using std::setfill;
using std::ifstream;
using std::ofstream;
using std::cerr;
using std::vector;

int main(int argc, const char *argv[])
{
    bool showtokens=false, showparsed=false, showlabels=false, ismainfile=false; // flags passadas para o ligador

    deque<Exception> exceptions; // lista de exceções

    string mainfilename; // nome do arquivo principal

    deque<Token> tokens; // tokens <- tokens gerados pelo arquivo principal
    //string line;

    for(int i = 1; i<argc; i++) // busca os argumentos
    {
        if(string(argv[i])=="-T")showtokens=true; // Se a flag -T estiver definida nos argumentos do ligador
        else if(string(argv[i])=="-L")showlabels=true; // Se a flag -L estiver definida nos argumentos do ligador
        else if(string(argv[i])=="-P")showparsed=true; // Se a flag -P estiver definida nos argumentos do ligador
        else if(string(argv[i]).starts_with("-m")) // Se é definido como arquivo principal
        {
            if(!ismainfile) // Se nenhum mainfile foi declarado ainda
            {
                ismainfile = true;
                mainfilename = string(argv[i]).substr(2); // armazena o nome do arquivo
                ifstream mainfile(mainfilename, std::ios::binary | std::ios::ate); // abre o arquivo

                if (!mainfile.is_open()) // Gera um erro se o arquivo não estiver aberto
                {
                    cerr << "Couldn't open Mainfile.\n";
                    return 1;
                }

                // Determina o tamanho do arquivo
                std::streamsize filesize = mainfile.tellg();
                mainfile.seekg(0, std::ios::beg);

                if(filesize == 0) // Gera um erro se o arquivo estiver vazio
                {
                    mainfile.close();
                    cerr << "Mainfile file is empty.\n";
                    return 1;
                }

                // Cria uma string com o tamanho apropriado
                std::string buffer(filesize, '\0');

                // Lê o conteúdo do arquivo na string
                if (!mainfile.read(&buffer[0], filesize)) {
                    std::cerr << "Erro ao ler o arquivo" << std::endl;
                    return 1;
                }

                // Fecha o arquivo
                mainfile.close();

                Result<deque<Token>> res = tokenize(buffer, mainfilename);

                if(res.exceptions.size() > 0)
                {
                    for(Exception e: res.exceptions)
                    {
                        exceptions.push_back(e);//cout << errors[e.errcode] << " " << e.msg << endl;
                    }
                    // return 1;
                }
                
                tokens = res.value;

            }
            else
            {
                // Se mais de um arquivo é declarado como mainfile
                cerr << "Multiple declaration of mainfile, only one mainfile must be declared.\n";
                return 1;
            }
        }
        else // Se o argumento passado para o ligador é inválido, o mesmo é ignorado
        {
            cout << "Argument ignored '" << argv[i] << "'.\n";
        }
    }

    if(!ismainfile)
    {
        cerr << "\nError: main file not declared.\nUsage: vm_as -m<main_file_name> [-T showtokens] [-L showlabels] [-P showparsed]\n";
        return 0;
    }

    // "Realoca" os tokens de dados para a parte superior antes de fazer o parsing
    int shift_data = 0, last_tmp_size = 0;
    string current_filename;
    for(auto it = tokens.begin(); it != tokens.end(); it++)
    {
		if(it->type == FILENAME)
		{
			current_filename = it->name;
		}
        else if (it->type == CURRENT_LINE && \
			(it + 1) != tokens.end() && \
			(it + 1)->type == DECLABEL && \
			(it + 2) != tokens.end() && \
			(it + 2)->type == DEFINE_WORD)
		{
            shift_data++;

			auto start = it;
            auto end = std::find_if(it, tokens.end(), [](const Token& elem) {
                return elem.type == NEWLINE || elem.type == ENDOFPROGRAM || elem.type == ENDOFFILE;
            });

            if (end != tokens.end() && end->type == NEWLINE) {
                ++end; // Incluir NEWLINE
            }

            // Move os elementos para o início
            std::deque<Token> temp(start, end);
			temp.push_front(Token(FILENAME, current_filename));
            tokens.erase(start, end);
            tokens.insert(tokens.begin(), temp.begin(), temp.end());

            // Ajusta o iterador para continuar a verificação corretamente
            it = tokens.begin() + temp.size() + last_tmp_size;
            last_tmp_size += temp.size();
        }
    }
    tokens.push_back(Token(ENDOFPROGRAM, "EOP"));

    if(showtokens) // Mostra os tokens caso a flag -T tenha sido definida nos argumentos do ligador
    {
        cout << "Tokens:\n";
        for(Token tk: tokens)
        {
            cout << tk << endl;
        }
        cout << "\n";
    }

    Result<deque<unsigned short>> res = parse(tokens, showlabels); // Faz o parsing e retorna um result

    cout << endl;
    for(Exception e: res.exceptions) exceptions.push_back(e); // Armazena as exceções

    if(exceptions.size() > 0) // Caso tenha alguma exceção, mostra a mesma e retorna
    {
        for(Exception e: exceptions)
        {
            std::cerr << e << endl; //errors[e.errcode] << " " << e.msg << endl;
        }
        return 1;
    }
    else // Se não tiver nenhuma exceção
    {
        deque<unsigned short> prog = res.value; // Armazena o código do programa
        prog.push_front((unsigned short)shift_data); // insere no começo a quantidade de dados

        if(showparsed) // se a flag -P foi definida para o ligador, mostra em hexadecimal os dados gerados
        {
            cout << "Parsed:\n";
            for(unsigned short inst: prog)
            {
                cout << setw(4) << setfill('0') << std::hex << inst << " ";
            }
            cout << "\n";
        }

        // remove extensão se tiver
        size_t indexof = mainfilename.find_last_of(".");
        if(indexof!=string::npos) mainfilename = mainfilename.substr(0, indexof);
        
        // remove diretório se tiver, usar / ao invés de \\ para o linux
        indexof = mainfilename.find_last_of("/");
        if(indexof!=string::npos) mainfilename = mainfilename.substr(indexof+1);

        cout << "\nfout >> "+mainfilename+".mem\n";
        
        ofstream fout(mainfilename+".mem", std::ios::binary); // Abre o arquivo binário com a extensão .mem
        
        if(!fout.is_open())
        {
            cerr << "Failed to create the file.\n";
            return 1;
        }
        
        vector<unsigned short> vec_prog;

        for (unsigned short val : prog) {
            vec_prog.push_back(val);
        }
        
        fout.write(reinterpret_cast<const char*>(vec_prog.data()), sizeof(unsigned short)*vec_prog.size()); // escreve os dados no arquivo

        fout.close(); // fecha o arquivo
    }

	return 0;
}