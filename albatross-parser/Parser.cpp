/*
 * Parser.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

#include <list>
using namespace std;

#include "Parser.h"

/* albatros-common */
#include <ParserException.h>

SyntaxTree* Parser::parse(Tokenizer::token_iterator & iterator) {
	list<Rule*> rules;

	Rule* returned;
	while((returned = parse_rule(iterator)) != nullptr) {
		rules.push_back(returned);
	}

	return new SyntaxTree(rules, rules.front()->getLine());
}
Rule* Parser::parse_rule(Tokenizer::token_iterator & iterator) {
	Token* identifier = *iterator++;
	if(identifier != Token::Type::Identifier)
		return nullptr;

	Token* t = *iterator++;
	if(t != Token::Type::Colon)
		return nullptr;

	Alternation* alternations;
	if((alternations = parse_alternations(iterator)) == nullptr)
		return nullptr;

	t = *iterator++;
	if(t != Token::Type::Semicolon)
		return nullptr;

	return new Rule(identifier->getString(), alternations, alternations->getLine());
}
Alternation* Parser::parse_alternations(Tokenizer::token_iterator & iterator) {
	MemberList* alternation_body;
	/* DON'T DO THIS!
	if((alternation_body = parse_alternation_body(iterator)) == nullptr)
			return nullptr;
	*/
	alternation_body = parse_member_list(iterator);

	Token* t = *iterator;
	if(t != Token::Type::Or)
		return new Alternation(alternation_body, nullptr, (alternation_body != nullptr ? alternation_body->getLine() : 0));

	iterator++;

	Alternation* alternations;
	if((alternations = parse_alternations(iterator)) == nullptr)
		return nullptr;

	//I'm checking if alternation_body!=nullptr because an empty statement can be also (invalidly) placed before other meaningful alternations
	//TODO: an exception/warning should be thrown, though
	return new Alternation(alternation_body, alternations, (alternation_body != nullptr ? alternation_body->getLine() : 0));
}
MemberList* Parser::parse_member_list(Tokenizer::token_iterator & iterator) {
	IParsed* alternation_member;
	if((alternation_member = parse_member(iterator)) == nullptr)
		return nullptr;

	MemberList* alternation_body;
	/* DON'T DO THIS!
	 * if((alternation_body = parse_alternation_body(iterator)) == nullptr)
		return nullptr;*/
	alternation_body = parse_member_list(iterator);

	return new MemberList(alternation_member, alternation_body, alternation_member->getLine());
}
IParsed* Parser::parse_member(Tokenizer::token_iterator & iterator) {
	Token* t = *iterator;

	Member::Output output = Member::Output::FullToken;
	switch(t->getType()) {
	case Token::Type::LogicNotSign:
		output = Member::Output::This;
		iterator++;
		break;
	case Token::Type::BitwiseNotSign:
		output = Member::Output::Inverse;
		iterator++;
		break;
	case Token::Type::MinusSign:
		output = Member::Output::Nothing;
		iterator++;
		break;
	case Token::Type::BeginRepeat: {
			IParsed* r = parse_member_list(++iterator);
			r = new AdjustedList((MemberList*)r, AdjustedList::Adjustment::Repetition, r->getLine());
			if((*iterator) != Token::Type::EndRepeat)
				throw ParserException("expected ')' (end of repetition list), '" + (*iterator)->getString() + "' found instead", r->getLine());

			iterator++;
			return r;
		}
		break;
	case Token::Type::BeginOptional: {
			IParsed* r = parse_member_list(++iterator);
			r = new AdjustedList((MemberList*)r, AdjustedList::Adjustment::Optional, r->getLine());
			if((*iterator) != Token::Type::EndOptional)
				throw ParserException("expected ']' (end of optional list), '" + (*iterator)->getString() + "' found instead", r->getLine());

			iterator++;
			return r;
		}
		break;
	default:
		/* do nothing */
		break;
	}

	t = *iterator;
	Member* ret;
	if(t==Token::Type::Identifier) {
		const char* browser = t->getString().c_str();
		while(isupper(*browser))
			browser++;

		if(*browser=='\0') //the name of the identifier is all-caps, therefore TOKEN
			ret = new Member(Member::Type::TokenData, t->getString(), t->getLine(), output);
		else
			ret = new Member(Member::Type::Identifier, t->getString(), t->getLine(), output);
	} else if(t==Token::Type::String)
		ret = new Member(Member::Type::String, t->getString(), t->getLine(), output);
	else
		return nullptr;

	iterator++;
	return ret;
}
