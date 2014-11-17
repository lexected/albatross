/*
 * STree.c
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#include "STree.h"

#include <cctype>
#include <sstream>
#include <list>
using namespace std;

#include "EscapeSequence.h"
#include "GeneralException.h"

ISList::~ISList() {
	//DO NOT, repeating, DO NOT DEALLOCATE ELEMENTS OF COLLECTION
	for(ISElement* element : elements)
			delete element;
	//lol, don't read his coments, he's stupid :D
}
std::string ISList::generate(std::string indentation) {
	std::stringstream ss;

	if(!isTopList())
		ss << "(";

	auto it = this->elements.begin();
	auto end = this->elements.end();

	bool isfirst = true;
	ISList* tmp;
	while(it!=end && (((tmp = dynamic_cast<ISList*>(*it)) == nullptr) || tmp->isTopList())) {
		string o = (*it++)->generate(indentation);
		if(o.empty() || isfirst) {
			ss << o;
			isfirst = false;
		} else
			ss << " " << o;
	}

	indentation += "  ";
	while(it!=end) {
		string o = (*it++)->generate(indentation);
		if(o.empty() || (isfirst && !isTopList())) {
			ss << o;
			isfirst = false;
		} else
			ss << "\n" << indentation << o;
	}

	if(!isTopList())
		ss << ")";

	return string(ss.str());
}

inline bool mustBeQuoted(std::string s) {
	const char* c = s.c_str();
	while(*c!='\0') {
		switch(*c) {
		case ' ':
		case '(':
		case ')':

		case '\n':
		case '\r':
		case '\v':
		case '\f':
		case '\b':
			return true;
		}

		c++;
	}

	return false;
}
std::string ISTerminal::generate(std::string indentation) {
	if(this->string.empty())
		return "\"\"";
	else if(mustBeQuoted(this->string))
		return "\"" + EscapeSequence::escape(this->string) + "\"";
	else
		return this->string;
}

enum ParserState {
	Whitespace,
	Terminal,
	String
};
ISElement* STree::parse(std::istream & is) {
	istreambuf_iterator<char> eos; //end of stream
	istreambuf_iterator<char> it(is.rdbuf());

	list<ISList*> lists;
	lists.push_back(new ISList()); //push a root element for the whole structure

	unsigned int line = 1;
	ParserState state = ParserState::Whitespace;
	bool escape = false;
	stringstream currtok;
	while(it!=eos) {
		char c = *it;

		if(c=='\n') {
			line++;

			it++; continue; //don't skip it++, or it'll be an endless loop
		}

		switch(state) {
		case ParserState::Whitespace:
			if(c=='"')
				state = ParserState::String;
			else if(c=='(')
				lists.push_back(new ISList());
			else if(c==')') {
				ISList* popped = lists.back();
				lists.pop_back();
				if(lists.empty())
					throw GeneralException("Too many closing braces ( ')' ) in list source");

				lists.back()->elements.push_back(popped);
			} else if(isspace(c)) {
				/* don't do anything */
			} else {
				currtok << c;
				state = ParserState::Terminal;
			}
			break;
		case ParserState::String:
			if(escape) {
				currtok << EscapeSequence::unescape(c, line);
				escape = false;
			} else if(c=='\\') {
				escape = true;
			} else if(c=='"') {
				lists.back()->elements.push_back(new ISTerminal(currtok.str()));
				currtok.str("");

				state = ParserState::Whitespace;
			} else {
				currtok << c;
			}
			break;
		case ParserState::Terminal:
			if(isgraph(c) && c!='"' && c!= '(' && c!= ')')
				currtok << c;
			else {
				lists.back()->elements.push_back(new ISTerminal(currtok.str()));
				currtok.str("");

				state = ParserState::Whitespace;
				continue; //skipping iteration, see?
			}
			break;
		}
		it++;
	}

	if(lists.size()!=1)
		throw GeneralException("Missing a closing brace ( ')' ) at the end of list source");


	return lists.back()->elements.front();
}
void STree::generate(std::ostream & os, ISElement* elements) {
	os << elements->generate("") << '\n';
}
