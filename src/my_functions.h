#ifndef _MY_FUNCTIONS_H_
#define _MY_FUNCTIONS_H_

#include <string>
#include <cctype>
// #include <stdexcept>
// #include <iostream> // só para debug

bool isNumber(std::string value) // verifica se a string é um número
{
	if(value.size()==0)
		return false;

	for(int i = 0; i < value.size(); i++)
		value[i] = std::tolower(value[i]);
    
	if(value[0] == '$' && value.size()>1)
	{
		std::string aux;
		aux+=value[0];
		for(int i=1; i<value.size(); i++)
		{
			if(std::isdigit(value[i]) || (value[i] >= 'a' && value[i] <= 'f'))
			{
				aux += value[i];
			}
		}

		return value == aux;
	}
	else if(value[0] == '%' && value.size()>1)
	{
		std::string aux;
		aux+=value[0];
		for(int i=1; i<value.size(); i++)
		{
			if(value[i] == '0' || value[i] == '1')
			{
				aux += value[i];
			}
		}

		
		return value == aux;
	}
	else if(value[0]=='&' && value.size()>1)
	{
		std::string aux;
		aux+=value[0];
		for(int i=1; i<value.size(); i++)
		{
			if(value[i]>='0' && value[i] <= '7')
			{
				aux += value[i];
			}
		}

		return value == aux;
	}
	else
	{
		std::string aux = "";
		
		for(int i=0; i<value.size(); i++)
		{
			if(std::isdigit(value[i]))
			{
				aux += value[i];
			}
		}
        // std::cout << "Value: "<< value << " Aux: " << aux << std::endl;
		return value == aux;
	}

	return false;
}

// int strToInt(std::string value, int base) // função substituida por uma função padrão -> std::stoi
// {
// 	if(value.size()==0)
// 	{
// 		throw std::runtime_error("Error at strToInt: String value was expected but found empty string");
// 	}

// 	switch(base)
// 	{
// 		case 2:
// 		{
// 			int ret = 0;
// 			for(int i = 0; i < value.size(); i++)
// 			{
// 				ret*=2;
// 				if(value[i]=='0' || value[i]=='1')
// 				{
// 					ret += (value[i]&0xf);
// 				}
// 				else
// 				{
// 					i=value.size();
// 					throw std::runtime_error(std::string("Error at strToInt:")+" Unexpected character "+value[i]+" for base 2 conversion");
// 				}
// 			}
// 			return ret;
// 		}
// 		break;
// 		case 8:
// 		{
// 			int ret = 0;
// 			for(int i = 0; i < value.size(); i++)
// 			{
// 				ret*=8;
// 				if(value[i]>='0' && value[i]<='7')
// 				{
// 					ret += (value[i]&0xf);
// 				}
// 				else
// 				{
// 					i=value.size();
// 					throw std::runtime_error(std::string("Error at strToInt:")+" Unexpected character "+value[i]+" for base 8 conversion");
// 				}
// 			}
// 			return ret;
// 		}
// 		break;
// 		case 10:
// 		{
// 			int ret = 0;
// 			for(int i = 0; i < value.size(); i++)
// 			{
				
// 				if(std::isdigit(value[i]))
// 				{
// 					ret*=10;
// 					ret += (value[i]&0xf);
// 				}
// 				else
// 				{
// 					i=value.size();
// 					throw std::runtime_error(std::string("Error at strToInt:")+" Unexpected character "+value[i]+" for base 10 conversion");
// 				}
// 			}
// 			return ret;
// 		}
// 		break;
// 		case 16:
// 		{
// 			int ret = 0;
// 			for(int i = 0; i < value.size(); i++)
// 			{
// 				ret*=16;
// 				if(std::isdigit(value[i]))
// 				{
// 					ret += (value[i]&0xf);
// 				}
// 				else
// 				{
// 					switch(std::tolower(value[i]))
// 					{
// 						case 'a':ret+=10;break;
// 						case 'b':ret+=11;break;
// 						case 'c':ret+=12;break;
// 						case 'd':ret+=13;break;
// 						case 'e':ret+=14;break;
// 						case 'f':ret+=15;break;
// 						default:
// 							i=value.size();
// 							throw std::runtime_error(std::string("Error at strToInt:")+" Unexpected character "+value[i]+" for base 16 conversion");
// 					}
// 				}
// 			}
// 			return ret;
// 		}
// 		break;
// 		default:
// 		throw std::runtime_error(std::string("Error at strToInt:")+" Unexpected base of conversion "+std::to_string(base)+", valid bases (2,8,10,16)");
// 	}
// }

int toInt16(std::string value) // transforma uma string em int
{
	int ret = 0;
	switch(value[0])
	{
		case '$':
			ret = std::stoi(value.substr(1), nullptr, 16) & 65535; //strToInt(value.substr(1), 16) & 65535;
		break;
		case '%':
			ret = std::stoi(value.substr(1), nullptr, 2) & 65535; //strToInt(value.substr(1), 2) & 65535;
		break;
		case '&':
			ret = std::stoi(value.substr(1), nullptr, 8) & 65535; //strToInt(value.substr(1), 8) & 65535;
		break;
		default:
			ret = std::stoi(value, nullptr, 10) & 65535; //strToInt(value, 10) & 65535;
	}
	return ret;
}

#endif