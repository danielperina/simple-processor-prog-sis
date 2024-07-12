#ifndef __ERROR_HANDLING_H__
#define __ERROR_HANDLING_H__

#include <iostream>
#include <map>
#include <string>
#include <deque>

using std::map;
using std::deque;
using std::string;

extern const char *errors[];

struct Exception
{
	string msg;
	int errcode;

	Exception(string _msg, int _err);

	~Exception();

	friend std::ostream& operator<<(std::ostream& os, Exception& e);
};

template<typename T>
struct Result
{
	deque<Exception> exceptions;
	T value;
};

#endif