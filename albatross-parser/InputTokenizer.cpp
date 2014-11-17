/*
 * InputTokenizer.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#include <sstream>

#include "InputTokenizer.h"

/* albatros-common */
#include <STree.h>

using namespace std;

InputTokenizer::inputtoken_list InputTokenizer::parse(std::istream & i) {
	ISElement* token_list_element = STree::parse(i);
	ISList* token_list = dynamic_cast<ISList*>(token_list_element);
	if(token_list==nullptr)
		throw ParserException("Source token list is not valid (and I have no idea why)");

	inputtoken_list ret;
	for(ISElement* token : token_list->elements) {
		ISList* token_contents = dynamic_cast<ISList*>(token);
		if(token_contents==nullptr || token_contents->elements.size()!=3)
			throw ParserException("Invalid token found in source token list (it doesn't have three members or it's not a list)", /* hack, invalid line info! */token_contents->elements.size());

		auto it = token_contents->elements.begin();
		ISTerminal* type = dynamic_cast<ISTerminal*>(*it++);
		ISTerminal* string = dynamic_cast<ISTerminal*>(*it++);
		ISTerminal* line = dynamic_cast<ISTerminal*>(*it++);
		if(type==nullptr || string==nullptr || line==nullptr)
			throw ParserException("Invalid token found in source token list (it doesn't contain three terminal members)");

		stringstream linestream(line->getString());
		unsigned int linenumber;
		linestream >> linenumber;
		ret.push_back({ type->getString(), string->getString(), linenumber });
	}

	return ret;
}
