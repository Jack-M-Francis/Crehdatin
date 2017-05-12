#pragma once

#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <iomanip>

#include <WebsiteFramework/WebsiteFramework.h>

std::string escapeHtml(const std::string& s);

std::string percentDecode(const std::string& SRC);
std::string percentEncode(const std::string &value);

enum InputError{
	NoError = 0,
	ContainsNewLine,
	ContainsNonNormal,
	IsMalformed,
	IsEmpty,
	IsTooLarge
};

enum InputFlag{
	AllowStrictOnly = 0,
	AllowNewLine = 1 << 0,
	AllowNonNormal = 1 << 1,//normal is defined as A-Z, a-z, 0-9, _, and -
	AllowBlank = 1 << 2,
	DontCheckInputContents = 1 << 3
};

InputError isValidText(const std::string& s, InputFlag flags);

inline InputFlag operator|(InputFlag a, InputFlag b){
	return static_cast<InputFlag>(static_cast<std::underlying_type_t<InputFlag>>(a) | static_cast<std::underlying_type_t<InputFlag>>(b));
}
inline InputFlag operator&(InputFlag a, InputFlag b){
	return static_cast<InputFlag>(static_cast<std::underlying_type_t<InputFlag>>(a) & static_cast<std::underlying_type_t<InputFlag>>(b));
}

InputError getPostValue(cgicc::Cgicc* cgi, std::string& output, std::string name, std::size_t maximumSize, InputFlag inputFlag);

