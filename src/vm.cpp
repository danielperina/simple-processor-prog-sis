#include <iostream> // cerr, cin, cout, endl...
#include <fstream> // ifstream
#include <vector> 
#include <string>
#include <iomanip> // std::setfill, std::setw

using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::string;

using WORD = unsigned short;

#define MEMSIZE 128     // word amount
#define BYTE_AMT 2      // 2 bytes, 16 bits
#define HEADER_SIZE 1   // data amount

struct vm_attr
{
    WORD acc, ir, pc, lr;
    WORD mem[MEMSIZE];
};

void load(vm_attr &attr, string filename)
{
    ifstream file(filename, std::ios::binary);
    
    if (!file.is_open())
    {
         throw std::runtime_error("Couldn't open the specified file.");
    }
    
    std::streampos filesize = (MEMSIZE+HEADER_SIZE)*sizeof(unsigned short);
    vector<WORD> aux((MEMSIZE+HEADER_SIZE));
    file.read(reinterpret_cast<char*>(aux.data()), filesize);
    file.close();
    
    attr.pc = aux.front();
    aux.erase(aux.begin());

    for(int i = 0; i < MEMSIZE && i < aux.size(); i++)attr.mem[i]=aux[i];
} 
    
void showRegisters(vm_attr &attr)
{
    cout << "\nPC: " << attr.pc 
    << " IR: " << std::setfill('0') << std::setw(BYTE_AMT*2) << std::hex << attr.ir 
    << " LR: " << std:: setfill('0') << std::setw(BYTE_AMT*2) << std::hex << attr.lr
    << " ACC: " << std::setfill('0') << std::setw(BYTE_AMT*2) << std::hex << attr.acc << endl;
}

void showMemory(vm_attr &attr)
{
    for(int i=0; i < MEMSIZE; i++)
    {
        if(i%16==0)cout << endl;
        cout << std::setfill('0') << std::setw(BYTE_AMT*2) << std::hex << attr.mem[i] << " ";
    }
    cout << endl;
}

void run(vm_attr &attr, bool debug)
{
    bool running=true;
    while(running)
    {

        attr.ir=attr.mem[(attr.pc&0x7f)]; // instruction register <- mem[pc]
        if(debug)showRegisters(attr);

        switch((attr.ir>>12))
        {
            case 0: // load accumulator
            {
                attr.acc = attr.mem[(attr.ir&0x7f)]; // accumulator register <- mem[operand]
                attr.pc++;
            }
            break;
            case 1: // store accumulator
            {
                attr.mem[(attr.ir&0x7f)] = attr.acc; // mem[operand] <- acc
                attr.pc++;
            }
            break;
            case 2: // add with accumulator
            {
                attr.acc += attr.mem[(attr.ir&0x7f)]; // acc <- acc + mem[operand]
                attr.pc++;
            }
            break;
            case 3: // multiply with accumulator
            {
                attr.acc *= attr.mem[(attr.ir&0x7f)]; // acc <- acc * mem[operand]
                attr.pc++;
            }
            break;
            case 4: // divide accumulator by
            {
                if(attr.mem[(attr.ir&0x7f)]==0) throw std::runtime_error("Invalid division by zero.");
                attr.acc /= attr.mem[(attr.ir&0x7f)]; // acc <- acc / mem[operand]
                attr.pc++;
            }
            break;
            case 5: // subtract from accumulator
            {
                attr.acc -= attr.mem[(attr.ir&0x7f)]; // acc <- acc - mem[operand]
                attr.pc++;
            }
            break;
            case 6: // unconditional jump
            {
                attr.pc=(attr.ir&0x7f);
            }
            break;
            case 7: // jump if accumulator equals to zero
            {
                attr.pc=((signed short)attr.acc==0)?(attr.ir&0x7f):attr.pc+1;
            }
            break;
            case 8: // jump if accumulator greater than zero
            {
                attr.pc=((signed short)attr.acc>0)?(attr.ir&0x7f):attr.pc+1;
            }
            break;
            case 9: // jump if accumulator less than zero
            {
                attr.pc=((signed short)attr.acc<0)?(attr.ir&0x7f):attr.pc+1;
            }
            break;
            case 10: // print word
            {
                cout << "print word: " << std::dec << attr.mem[(attr.ir&0x7f)] << endl;
                attr.pc++;
            }
            break;
            case 11: // read word
            {
                cout << "read word: ";
                std::cin >> attr.mem[(attr.ir&0x7f)];
                attr.pc++;
            }
            break;
            case 12: // jump if accumulator greater to zero
            {
                running=false;
            }
            break;
            case 13: // jump subroutine
            {
            	attr.lr = attr.pc+1;
            	attr.pc=(attr.ir&0x7f);
            }
            break;
            case 14: // return subroutine
            {
            	attr.pc = attr.lr;
            }
            break;
            default:
                attr.pc++;
        }
        
        if(attr.pc>127)
        {
            running=false;
            cout << "PC exceeded memory capacity.\n";
        }
    }
    cout << "program finished.\n";
}

void vm(vm_attr &attr) // inicializa os registradores e memória
{
    attr.pc = 0;
    attr.ir = 0;
    attr.acc = 0;
    attr.lr = 0;
    
    for(int i = 0; i < MEMSIZE; i++)attr.mem[i]=0;
}

int main(int argc, const char * argv[])
{
    
    if(argc == 1)
    {
        cout << "Usage: vm <filename> [-d debug]\n";
        return 0;
    }
    
    bool debug=false;
    for(int i=2; i < argc;i++)
    {
        if(string(argv[i])=="-d")debug=true;
    }
    
    vm_attr attr;
    vm(attr); // inicializa os registradores e zera a memória

    try
    {
        load(attr, argv[1]); // carrega o programa
        run(attr, debug); // roda o programa
        
        if(debug)showMemory(attr);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << endl;
        return 1;
    }
    
    
    return 0;
}
