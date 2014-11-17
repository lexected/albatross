/*
 * EscapeSequence.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#include <string>
#include <sstream>
using namespace std;

#include "EscapeSequence.h"
#include "GeneralException.h"

char EscapeSequence::unescape(char c, unsigned int line) {
	switch(c) {
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 'v':
			return '\v';
		case 'f':
			return '\f';
		case 'b':
			return '\b';
		case '\\':
			return '\\';
		case '"':
			return '"';
		default:
			throw GeneralException(string() + "Invalid escape sequence '\\" + c + "'");
		}
}
std::string EscapeSequence::escape(std::string s) {
	const char* cstr = s.c_str();
	stringstream ss;

	while(*cstr!='\0') {
		switch(*cstr) {
		case '\\':
			ss << "\\\\";
			break;
		case '"':
			ss << "\\\"";
			break;
		case '\n':
			ss << "\\n";
			break;
		case '\r':
			ss << "\\r";
			break;
		case '\v':
			ss << "\\v";
			break;
		case '\f':
			ss << "\\f";
			break;
		case '\b':
			ss << "\\b";
			break;
		default:
			ss << *cstr;
			break;
		}

		cstr++;
	}

	return ss.str();
}
