/*
 * SyntaxMatcher.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#include "SyntaxMatcher.h"
#include "ParserException.h"

#include <typeinfo>
#include <sstream>
#include <cctype>
#include <algorithm>
using namespace std;

ISList* SyntaxMatcher::match(InputTokenizer::inputtoken_iterator & it) {
	ISList* ast = match_rule(it, syntax->elements.front()->name, 0);
	return ast;
}
ISList* SyntaxMatcher::match_rule(InputTokenizer::inputtoken_iterator & it, std::string name, unsigned int line) {
	ISList* ret = nullptr;

	auto found = find_if(syntax->elements.begin(), syntax->elements.end(), [&name](Rule* element) { return element->name==name; });
	if(found==syntax->elements.end())
		throw ParserException("Unknown production rule '" + name + "' used in the syntax file", line);

	Alternation* curralt = (*found)->alternations;
	InputTokenizer::inputtoken_iterator savedit = it;
	while(curralt!=nullptr) {
		ISList* returned = match_member_list(it, curralt->alternation_body);
		if(returned==nullptr)
			it = savedit;
		else {
			ret = new ISList();
			ret->elements.push_back(new ISTerminal(name));
			ret->elements.insert(ret->elements.end(), returned->elements.begin(), returned->elements.end());

			returned->elements.clear();
			delete returned;
			break;
		}

		curralt = curralt->alternations;
	}

	return ret;
}
ISList* SyntaxMatcher::match_member_list(InputTokenizer::inputtoken_iterator & it, MemberList* body) {
	ISList* ret = new ISList();

	MemberList* currbody = body;
	while(currbody!=nullptr) {
		ISElement* returned = match_member(it, currbody->member);
		if(returned==nullptr)
			return nullptr;

		ret->elements.push_back(returned);
		currbody = currbody->alternation_body;
	}

	return ret;
}
ISElement* SyntaxMatcher::match_member(InputTokenizer::inputtoken_iterator & it, IParsed* member) {
	InputToken t = *it;

	AdjustedList* al;
	if((al = dynamic_cast<AdjustedList*>(member)) != nullptr) {
		ISList* ret_list = new ISList(true);
		ISList* tmp_list;

		if(al->getAdjustment()==AdjustedList::Adjustment::Repetition) {
			while((tmp_list = match_member_list(it, al->list)) != nullptr) {
				ret_list->elements.insert(ret_list->elements.end(), tmp_list->elements.begin(), tmp_list->elements.end());

				tmp_list->elements.clear();
				delete tmp_list;
			}

			if(ret_list->elements.empty()) {
				delete ret_list;
				ret_list = nullptr;
			}
		} else { //al->getAdjustment()==AdjustedList::Adjustment::Optional
			if((tmp_list = match_member_list(it, al->list)) != nullptr) {
				ret_list->elements = tmp_list->elements;

				tmp_list->elements.clear();
				delete tmp_list;
			}
		}
		return ret_list;
	} else { //dynamic_cast<ParsedToken*>(member) != nullptr
		Member* p = dynamic_cast<Member*>(member);
		ISList * ret_list;

		switch(p->getType()) {
		case Member::Type::TokenData:
				if(t.type==p->getString()) {
					stringstream ss;
					ss << t.line;
					ret_list = new ISList({ new ISTerminal(t.type), new ISTerminal(t.string), new ISTerminal(ss.str()) });

					it++;
				} else
					return nullptr;
			break;
		case Member::Type::Identifier:
			if((ret_list = match_rule(it, p->getString(), member->getLine())) == nullptr)
				return nullptr;
			break;
		case Member::Type::String:
			if(t.string==p->getString()) {
				stringstream ss;
				ss << t.line;
				ret_list = new ISList({ new ISTerminal(t.type), new ISTerminal(t.string), new ISTerminal(ss.str()) });

				it++;
			} else
				return nullptr;
			break;
		case Member::Type::EndOfStream:
			throw ParserException("Unexpected end of token stream", t.line);
			break;
		default:
			throw GeneralException("Unknown ParsedToken::Type <= this is an internal error of this parser and you can't do much about it");
			break;
		}

		ISElement* ret;
		switch(p->getOutput()) {
		case Member::Output::Nothing:
			ret = new ISEmpty();

			delete ret_list;
			break;
		case Member::Output::This:
			if(p->getType()==Member::Type::Identifier || p->getType()==Member::Type::TokenData)
				ret = new ISTerminal(dynamic_cast<ISTerminal*>(ret_list->elements.front())->getString());
			else
				ret = new ISTerminal(p->getString());

			delete ret_list;
			break;
		case Member::Output::Inverse:
			if(p->getType()==Member::Type::TokenData) {
				auto second_element = ret_list->elements.begin();
				second_element++;
				ret = new ISTerminal(dynamic_cast<ISTerminal*>((*second_element))->getString());
			}
			else if(p->getType()==Member::Type::Identifier) {
				auto copy_start = ret_list->elements.begin();
				copy_start++;

				ISList* to_be_returned = new ISList(true);
				to_be_returned->elements = decltype(ret_list->elements)(copy_start, ret_list->elements.end());
				ret = to_be_returned;

				ret_list->elements.clear();
				delete ret_list;
			} else { //p->getType()==ParsedToken::Type::String
				ISTerminal* was_to_be_returned_token = dynamic_cast<ISTerminal*>(ret_list->elements.front());
				ret = new ISTerminal(was_to_be_returned_token->getString());

				delete ret_list;
			}
			break;
		case Member::Output::FullToken:
			/* do nothing, just return the matched token as it is */
			ret = ret_list;
			break;
		}

		return ret;
	}
}
