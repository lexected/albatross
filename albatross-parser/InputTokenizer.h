/*
 * InputTokenizer.h
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#ifndef INPUTTOKENIZER_H_
#define INPUTTOKENIZER_H_

#include <string>
#include <istream>
#include <list>

/* albatros-common */
#include <ParserException.h>

struct InputToken {
	std::string type;
	std::string string;
	unsigned int line;
};

class InputTokenizer final {
public:
	typedef std::list<InputToken> inputtoken_list;
	typedef inputtoken_list::iterator inputtoken_iterator;

	static inputtoken_list parse(std::istream & i);
private:
	InputTokenizer() = default;
	~InputTokenizer() = default;
};

#endif /* INPUTTOKENIZER_H_ */
