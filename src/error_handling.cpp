#include "error_handling.h"

std::ostream& operator<<(std::ostream& os, Exception& e) // função para imprimir uma exceção
{
    os << errors[e.errcode] << " " << e.msg;

    return os;
}

const char *errors[] = { // tipos de erros
	"Invalid base conversion",
	"Unexpected character",
	"Multiple lable declaration",
	"Unexpected operand",
	"Undeclared label",
	"Strange operator"
};

Exception::Exception(string _msg, int _err) // construtor de uma Exception
{
	msg = _msg;
	errcode = _err;
}

Exception::~Exception() {} // Destrutor