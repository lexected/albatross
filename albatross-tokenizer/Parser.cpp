/*
 * Parser.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: lexected
 */


using namespace std;

#include "Parser.h"

Parser::rule_list Parser::parse(Tokenizer::token_iterator & it) {
	rule_list* list;
	if((list = parse_tokenrulelist(it)) == nullptr)
		return rule_list();
	else {
		Token* t  = *it;
		if(t==Token::Type::Semicolon)
			it++; //this should make *it==Token::Type::EndOfFile, of course only if "end;" was really on the end of file

		return *list; //return the parsed list anyway
	}
}

TokenForm* Parser::parse_tokenform(Tokenizer::token_iterator & it) {
	unsigned int line = (*it)->getLine();
	TokenRule::Importance importance = parse_importance(it);
	TokenRule::Action attributes = parse_actions(it);

	Token* t = *it;
	string name = t->getString();
	const char* browser = t->getString().c_str();
	while(*browser!='\0' && ((isalpha(*browser) && isupper(*browser)) || !isalpha(*browser)))
		browser++;
	if(*browser!='\0')
		throw ParserException("Token name is not in upper case: '" + name + "'", line);

	it++;
	if((t = *it)!=Token::Type::Colon)
		return nullptr;

	it++;
	if((t = *it)!=Token::Type::String)
		return nullptr;

	string regex = t->getString();
	size_t start = 0;
	while((start = regex.find('\\', start)) != string::npos) {
		regex.replace(start, 1, "\\\\");
		start += 2;
	}

	it++; //point at the semicolon at the end
	return new TokenForm(name, attributes, importance, regex, line);
}
TokenCompound* Parser::parse_tokencompound(Tokenizer::token_iterator & it) {
	unsigned int line = (*it)->getLine();

	Token* t;

	TokenCompound::Type type;
	t = *it;
	if(t->getString()=="begin")
		type = TokenCompound::Type::Simple;
	else if(t->getString()=="repeat")
		type = TokenCompound::Type::Repetetive;
	else
		return nullptr;
	it++;

	TokenRule::Importance importance = parse_importance(it);
	TokenRule::Action actions = parse_actions(it);

	if((t = *it)!=Token::Type::Identifier)
		return nullptr;

	string name = t->getString();
	const char* browser = t->getString().c_str();
	while(*browser!='\0' && ((isalpha(*browser) && isupper(*browser)) || !isalpha(*browser)))
		browser++;
	if(*browser!='\0')
		throw ParserException("Token name is not in upper case: '" + name + "'", line);

	it++;
	if((t = *it)!=Token::Type::Colon)
		return nullptr;

	it++;
	Parser::rule_list* list;
	if((list = Parser::parse_tokenrulelist(it))==nullptr)
		return nullptr;

	return new TokenCompound(name, actions, importance, type, *list, line);
}
Parser::rule_list* Parser::parse_tokenrulelist(Tokenizer::token_iterator & it) {
	list<TokenRule*>* list = new std::list<TokenRule*>();

	Token* t;
	while((t = *it)==Token::Type::Identifier) {
		Tokenizer::token_iterator copy = it;
		TokenRule* toBeAdded;

		if(t->getString()=="end") {
			it++;
			break;
		} else if((toBeAdded = parse_tokencompound(copy))!=nullptr) {
			list->push_back(toBeAdded);
			it = copy;
		} else if((toBeAdded = parse_tokenform(it))!=nullptr) {
			list->push_back(toBeAdded);
		} else {
			return nullptr;
		}

		if((t = *it)!=Token::Type::Semicolon)
			return nullptr;
		it++;
	}

	return list;
}


TokenRule::Action Parser::parse_actions(Tokenizer::token_iterator & it) {
	TokenRule::Action ret = TokenRule::Action::None;
	Tokenizer::token_iterator old = it;

	Token* t;
	while((t = *it)==Token::Type::Identifier) {
		if(t->getString()=="consume") {
			if((ret&TokenRule::Action::Consume)!=0)
				throw ParserException("'consume': reattribution of token rule", t->getLine());
			ret = ret | TokenRule::Action::Consume;
		} else if(t->getString()=="tokenize") {
			if((ret&TokenRule::Action::Tokenize)!=0)
				throw ParserException("'tokenize': reattribution of token rule", t->getLine());
			ret = ret | TokenRule::Action::Tokenize;
		} else if(t->getString()=="produce") {
			if((ret&TokenRule::Action::Produce)!=0)
				throw ParserException("'produce': reattribution of token rule", t->getLine());
			ret = ret | TokenRule::Action::Produce;
		} else {
			goto outloop;
		}

		it++;
	}
	outloop:

	if(it==old)
		ret = TokenRule::Action::Consume | TokenRule::Action::Tokenize;

	return ret;
}
TokenRule::Importance Parser::parse_importance(Tokenizer::token_iterator & it) {
	TokenRule::Importance ret = TokenRule::Importance::Optional;

	Token* t = *it;
	if(t->getString()=="optional") {
		ret = ret | TokenRule::Importance::Optional;
		it++;
	} else if(t->getString()=="required") {
		ret = ret | TokenRule::Importance::Required;
		it++;
	} else if(t->getString()=="forbidden") {
		ret = ret | TokenRule::Importance::Forbidden;
		it++;
	}

	return ret;
}
